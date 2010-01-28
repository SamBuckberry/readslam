<?php

class MC_Hammer
{
	var $link;
	var $table;
	var $verbose;
	
	function MC_Hammer($link)
	{
		$this->link = $link;
		$this->table = '';
		$this->verbose = false;
	}
	
	//Determine the mC error rate using uncollapsed chloroplast reads (should have no mCs, ie all Cs should read as Ts). Incorporates both machine error and BS conversion failure.
	function get_error_rate($table)
	{
		$error = 0;
		$total = 0;

		$d = mysql_query("select sum(fc), sum(ftotal) from $table where fbase='C'", $this->link);
		$r = mysql_fetch_row($d);
		$error += $r[0];
		$total += $r[1];

		$d = mysql_query("select sum(rc), sum(rtotal) from $table where rbase='C'", $this->link);
		$r = mysql_fetch_row($d);
		$error += $r[0];
		$total += $r[1];

		return $error / $total;
	}	
	
	//Run mc hammer on the specified table. Use full db.table syntax
	function hammer($table, $error_rate = 0.03, $falsepos = 0.05, $verbose=false)
	{
		$this->verbose = $verbose;
		
		if ($this->verbose) echo "It's hammer time for table $table\n";
		if ($this->verbose) echo "\tCytosine call error (failed conversion + read error): " . round(100 * $error_rate, 4) . "%\n";
		$this->table = $table;
		$probabilities = $this->build_probability_table($error_rate);
		$this->refine($probabilities, '+', $falsepos, $error_rate);
		$this->refine($probabilities, '-', $falsepos, $error_rate);
	}	
				
	//Build a table of probabilities from the binomial distribution
	function build_probability_table($error_rate)
	{
		$limit = 60;
		$array = array();
		
		for ($height=1; $height<=$limit; $height++)
		{
			for ($mcs=1; $mcs<=$height; $mcs++)
			{
				$_height = 1.0 * $this->factorial($height);
				$_mc     = 1.0 * $this->factorial($mcs);
				$_diff   = 1.0 * $this->factorial($height - $mcs);
				
				$array[$height][$mcs] = ($_height / ($_mc * $_diff)) * (pow($error_rate,$mcs)) * (pow((1-$error_rate),($height-$mcs)));
			}
		}
		return $array;
	}
		
	//Factorial function, not a good idea to try and calculate factorial > 100
	function factorial($num)
	{
		$result = 1;
		
		while ($num > 0)
		{
			$result *= $num;
			$num--;
		}
		return $result;
	}
	
	//Refine parameters until exact % coverage is determined
	function refine($probabilities, $strand, $falsepos)
	{
		if ($this->verbose) echo "\tHammering strand: $strand\n";		
		if ($this->verbose) echo "\t\tBegin with 100% coverage (mC/C)\n";

		$strand = ($strand == '+' || $strand == 'f') ? 'f' : 'r';
	
		$coverage = 1.0;
		$last_coverage = 0.0;
		$counter = 0;
		
		while ($coverage != $last_coverage) {
			$counter++;
			if ($this->verbose) echo "\t\tIteration $counter: ";
	
			$cutoff = $this->get_cutoff($falsepos, $coverage);

			if ($this->verbose) echo "slip: " . round(100 * $cutoff,4) . "% ";
	
			$bins = $this->build_bins($probabilities, $cutoff);
			$last_coverage = $coverage;
			$coverage = $this->coverage($bins, $strand);
			
			if ($this->verbose) echo "coverage: " . round(100 * $coverage, 4) . "%\n";
		}
		if ($this->verbose) echo "\t\tUpdating database...";
		$this->update_database($bins, $strand);
		if ($this->verbose) echo "done\n";
	}

	//Sliprate is the max % of normal Cs to allow through as false positive mC calls and still satisfy cutoff
	function get_cutoff($falsepos, $coverage)
	{
		if ((1+$falsepos) * $coverage >= 1) return 1;
		return ($falsepos * $coverage) / (1-$coverage);
	}
	
	//Build bins from probability table array
	function build_bins($probabilities, $cutoff)
	{
		$bins = array();
		
		foreach ($probabilities as $height => $probs)
		{
			foreach ($probs as $mcs => $prob)
			{
				if ($prob <= $cutoff)
				{
					if (!isset($bins[$mcs])) $bins[$mcs] = array('mc'=>$mcs, 'min'=>$height, 'max'=>$height);
					$bins[$mcs]['max'] = $height;
					break;
				}
			}
		}
		
		$temp = array();
		
		foreach ($bins as $bin)
		{
			$temp[] = $bin;
		}
		return $temp;
	}
		
	//Determine % mC coverage using bins
	function coverage($bins, $strand)
	{		
		$strand = ($strand == '+' || $strand == 'f') ? 'f' : 'r';
		if (count($bins) == 0) return 0;
		
		//Build the requirement components of the query
		$where = array();
			
		foreach ($bins as $bin)
		{
			$where[] = "({$strand}c >= {$bin['mc']} and {$strand}total between {$bin['min']} and {$bin['max']})";
		}
		$where = implode(" or ", $where);
		
		//Add the final component
		$max = $bins[count($bins)-1]['max'] + 1;
		$pct = $bins[count($bins)-1]['mc'] / $bins[count($bins)-1]['max'];
		
		$where .= " or ({$strand}total >= $max and {$strand}c / {$strand}total > $pct)";
		
		$d = mysql_query("select count(*) from {$this->table} where {$strand}base='C' and ($where)", $this->link);
		$r = mysql_fetch_row($d);
		$methylated = $r[0];
		
		$d = mysql_query("select count(*) from {$this->table} where {$strand}base='C'", $this->link);
		$r = mysql_fetch_row($d);
		$total = $r[0];
		
		return $methylated / $total;		
	}	
	
	//Insert mC calls to database (updates collapsed BS data)
	function update_database($bins, $strand)
	{
		if (count($bins) == 0) return;
		
		$strand = ($strand == '+' || $strand == 'f') ? 'f' : 'r';
		
		//Firstly, unset any existing mC calls
		$d = mysql_query("update {$this->table} set {$strand}call='N' where {$strand}call='C'", $this->link);
		
		//Build the requirement components of the query
		$where = array();
	
		$min = -1;
		$max = 0;
		
		//max and min refer to stack heights
		foreach ($bins as $bin)
		{
			if ($min == -1) $min = $bin['min'];
			if ($bin['min'] < $min) $min = $bin['min'];
			if ($bin['max'] > $max) $max = $bin['max'];
			
			$where[] = "({$strand}c >= {$bin['mc']} and {$strand}total between {$bin['min']} and {$bin['max']})";
		}
		$where = implode(" or ", $where);
		
		//Add the final component
		$max = $bins[count($bins)-1]['max'] + 1;
		$pct = $bins[count($bins)-1]['mc'] / $bins[count($bins)-1]['max'];
		
		$where .= " or ({$strand}total >= $max and {$strand}c / {$strand}total >= $pct)";
			
		mysql_query("update {$this->table} set {$strand}call='C' where {$strand}base='C' and ($where)", $this->link);
	}	
}

?>