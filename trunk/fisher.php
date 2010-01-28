<?php

//Returns a double
function fishers($a , $b, $c, $d)
{
	$det = 0.0;
	$temp = 0.0;
	$sample_size = 0.0;
	$pop = 0.0;
	
	$det = $a * $d - $b * $c;
	
	if ($det > 0)
	{
		$temp = $a;
		$a = $b;
		$b = $temp;
		$temp = $c;
		$c = $d;
		$d = $temp;
		$det = -$det;
	}
	$sample_size = $a + $b;
	$temp = $a + $c;
	$pop = $sample_size + $c + $d;

	return cdf_hypergeometric($a, $sample_size, $temp, $pop) + 
	comp_cdf_hypergeometric($a - 2 * $det / $pop, $sample_size, $temp, $pop);
}

?>

