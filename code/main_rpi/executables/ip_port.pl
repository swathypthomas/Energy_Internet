#!/usr/bin/perl
#@author swathy
#@date 26 Feb 20
#usage ./ip\&port.pl
#The code reads info.log captured from switch and mac_ip from arp-scan. It then takes mac and port details from info.log. The code finds 
#ip address in range of 192.168.0.100 to 192.168.0.255 assigned to master controllers(Rpi). It checks ports having 2 MAC address on same port.
#It maps these IP to ports for 2 MAC entries and within IP range. This gives peer ip and port combination in file ip_port

use strict; 
use warnings;
use File::Copy qw(copy);

my @lines;
my @switchMacLine;
my @switchMac;
my @arpIpMac;
my @switchIp = $ARGV[0];
truncate 'mac_port', 0;
truncate 'selected_mac_port', 0;
truncate 'peer_ip_port', 0;
truncate 'local_ip_port', 0;
open my $f1, '<', 'info.log' or die "Cannot open file info.log: $!\n";
open my $f2, '<', 'mac_ip' or die "Cannot open file mac_ip: $!\n";
open my $f3, '>>', 'mac_port' or die "Cannot open file mac_port: $!\n";
open my $f8, '>>', 'peer_ip_port' or die "Cannot open file peer_ip_port: $!\n";
open my $f18, '>>', 'local_ip_port' or die "Cannot open file local_ip_port: $!\n";
while (my $line1 = <$f1>)
{
  if ($line1 =~ /(Gi0\/)/)
  {
  	push (@lines, $line1);   # add it to the array.  	
  }
}
while (my $l = <$f2>)
{
	foreach (@switchIp)
 	 {
  		if ($l =~ $_)
 		{
  			push (@switchMacLine, $l);
  			#print "$line1\n";
  		}
  	 }
}
close $f1;
close $f2;
foreach (@lines) 
{
  my $mac = (split ' ', $_)[1];  
  my $portnotation = (split ' ', $_)[3];
  my $port = (split /\//, $portnotation)[-1];
  print $f3 "$mac $port\n";
}
foreach (@switchMacLine)
{
	my $sMac = (split ' ', $_)[1];
  	push (@switchMac, $sMac);
  	#print "$sMac\n";
}
close $f3;
my @pr;
my $sM;
open my $f9, '<', 'mac_port' or die "Cannot open file mac_port: $!\n";
foreach (@switchMac)
{
	$sM = $_;
	#$sM =~ s/-/:/g;
	#print "\n$sM\n";
	while (my $find = <$f9>)
	{
		#print "$find\n";
		if ($find =~ $sM)
		{
			my $prt = (split ' ', $find)[1];
			push (@pr, $prt);
			#print "$prt\n";
		}
	}
}
close $f9;
open my $f11, '<', 'mac_port' or die "Cannot open file mac_port: $!\n";
open my $f10, '>>', 'selected_mac_port' or die "Cannot open file selected_mac_port: $!\n";
while (my $mpl = <$f11>)
{
	#print "$mpl\n";
	foreach(@pr)
	{
		my $p1 = $_;
		my $p = (split ' ', $mpl)[1];
		if($p == $p1)
		{
			print $f10 "$mpl";
			#print "$mpl\n";
		}
	}
}
close $f9;
close $f10;


open my $f12, '<', 'mac_ip' or die "Cannot open file mac_ip: $!\n";

my $d = "[0-9A-Fa-f]";
my $dd = "$d$d";

while(my $line2 = <$f12>) 
{
	if($line2 =~ /(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})/) 
	{ 
		push(@arpIpMac, $line2);
		my $ip = $1;
		my $num = ( split /[.]/, $ip )[-1];
		#print "$ip\n";
		if($num > 99 && $num < 200)
		{
			my $mac2 = (split ' ', $line2)[1];
			#print "$mac2\n";
			open my $f7, '<', 'selected_mac_port' or die "Cannot open file selected_mac_port: $!\n";
			while(my $line3 = <$f7>)
			{
				if($line3 =~ $mac2)
				{
					my $port1 = (split ' ', $line3)[1];
					print $f8 "$ip $port1\n";
				}
			}
			close $f7;
		}  		  
	} 
}
close $f12;
close $f8;

copy 'mac_port', 'mac_port_cpy';

open my $f13, '<', 'mac_port' or die "Cannot open file mac_port: $!\n";
open my $f14, '<', 'mac_port_cpy' or die "Cannot open file mac_port_cpy: $!\n";
open my $f15, '>>', 'mac_port_nondup' or die "Cannot open file mac_port_nondup: $!\n";
while(my $line7 = <$f13>)
{
	my $port7 = (split ' ', $line7)[1];
	my $i = 0;
	while (my $line8 = <$f14>)
	{
		my $port8 = (split ' ', $line8)[1];
		if($port8 =~ $port7)
		{
			$i = $i + 1;
		}
	}
	if ($i < 2)
	{
		print $f15 "$line7";
		#print "$line7";
	}
}

close $f13;
close $f14;
close $f15;

my $mac5 = 0;
my $port5 = 0;
open my $f16, '<', 'mac_port_nondup' or die "Cannot open file mac_port_nondup: $!\n";
while(my $line5 = <$f16>)
{
	$mac5 = (split ' ', $line5)[0];
	$port5 = (split ' ', $line5)[1];
	foreach(@arpIpMac)
	{
		my $mac6 =(split ' ', $_)[1];
		if($mac6 =~ $mac5)
		{
			my $ip6 =(split ' ', $_)[0];
			my $num6 = ( split /[.]/, $ip6 )[-1];
			#print "$ip\n";
			if($num6 > 9 && $num6 < 100)
			{
				if(($num6 != 20) && ($num6 != 40) && ($num6 != 60) && ($num6 != 80) && ($num6 != 100) && ($num6 != 120))
				{
					print $f18 "$ip6 $port5\n";					
				}				
			}
		}
	}
}
close $f13;
close $f18;

unlink ('info.log');
unlink ('selected_mac_port');
unlink ('mac_ip');
unlink ('mac_port');
unlink ('mac_port_cpy');
unlink ('mac_port_nondup');



=begin
copy 'mac_port', 'mac_port_cpy';
open my $f4, '<', 'mac_port' or die "Cannot open file mac_ip: $!\n";
open my $f5, '>>', 'peer_mac_port' or die "Cannot open file ip_port: $!\n";
my $count;
my $p;
my $p1;
while (my $line4 = <$f4>)
{
	$p = (split ' ',$line4)[1];
	$count = 0;
	open my $f6, '<', 'mac_port_cpy' or die "Cannot open file mac_ip: $!\n";
	while (my $line41 = <$f6>)
	{
		$p1 = (split ' ',$line41)[1];
		if($p ==  $p1)
		{
			$count++;
		}
	}
	close $f6;
	if($count == 2)
	{
		print $f5 "$line4";
	}
}
close $f4;
close $f5;
=cut 