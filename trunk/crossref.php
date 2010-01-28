<?php


if ($argc != 3) die("Usage: php crossref reads.final reads.bed\n");

$count_a = 0;
$count_b = 0;
$overlap = 0;

$set = array();

//load in final file
$fp = fopen($argv[1], 'r');

echo "\nLoading file A\n";

while (!feof($fp))
{
	$fields = explode("\t", fgets($fp));

	if (count($fields) < 5) continue;
	
	if ($fields[2] == '+')
	{
		$tag = $fields[0] . $fields[1] . $fields[2] . $fields[3];
	}
	else
	{
		$tag = $fields[0] . $fields[1] . $fields[2] . $fields[4];
	}
	$set[$tag] = 0;
	
	if (++$count_a % 10000 == 0)
	{
		echo " $count_a $tag     \r";
	}	
}
fclose($fp);

//compare bed file
$fp = fopen($argv[2], 'r');

echo "\nLoading file B\n";

while (!feof($fp))
{
	$fields = explode("\t", fgets($fp));
	
	if (count($fields) < 5) continue;
	
	$fields[5] = trim($fields[5]);
	
	if ($fields[5] == '+')
	{
		$tag = $fields[3] . substr($fields[0],3) . $fields[5] . ($fields[1]);
	}
	else
	{
		$tag = $fields[3] . substr($fields[0],3) . trim($fields[5]) . ($fields[2]-1);
	}
	if (isset($set[$tag]))
	{
		$overlap++;
	}
	if (++$count_b % 10000 == 0)
	{
		echo " $count_b $tag     \r";
	}	
}
fclose($fp);

echo 
	"\n:: Results ::\n".
	"A size: $count_a\n".
	"B size: $count_b\n".
	"Overlap: $overlap\n"
;



$q = mysql_query("create table $db.stacks_{$table_name}_{$chr} ("
	."assembly char(2) not null,"
	."position int(10) unsigned default 0 not null,"
	."fbase char(1) default 'N' not null,"
	."fcall char(1) default 'N' not null,"
	."fa smallint unsigned default 0 not null,"
	."ft smallint unsigned default 0 not null,"
	."fc smallint unsigned default 0 not null,"
	."fg smallint unsigned default 0 not null,"
	."fn smallint unsigned default 0 not null,"
	."rbase char(1) default 'N' not null,"
	."rcall char(1) default 'N' not null,"
	."ra smallint unsigned default 0 not null,"
	."rt smallint unsigned default 0 not null,"
	."rc smallint unsigned default 0 not null,"
	."rg smallint unsigned default 0 not null,"
	."rn smallint unsigned default 0 not null,"
	."ftotal smallint unsigned default 0 not null,"
	."rtotal smallint unsigned default 0 not null"
.")", $link);

$q = mysql_query("create table $db.mc_new_{$table_name}_{$chr} ("
	."assembly char(2) not null,"
	."position int(10) unsigned default 0 not null,"
	."strand enum('+','-','.') default '.' not null,"
	."`class` enum('CG','CHG','CHH','N') default 'N' not null,"
	."mc smallint unsigned default 0 not null,"
	."h smallint unsigned default 0 not null"
.")", $link);

10      50478   +       CG      4       5
10      50515   +       CG      9       13
10      51652   +       CG      9       15
10      51868   +       CHH     1       3
10      53584   +       CG      4       8
10      53651   +       CHH     2       12
10      54773   +       CG      6       10
10      54774   +       CG      23      27
10      55063   +       CG      2       4
10      55070   +       CG      2       6



?>

