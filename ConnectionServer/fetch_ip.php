<?php
	/*
	* This file update the ip address for the robot arm
	* Return: the ip address format: 127.0.0.1:5000; return 0 if Failure
	* Example: fetch_ip.php?id=1
	*/
	header('Content-Type:text/plain');
	include "system.inc";
	/*
	* Retrieve data from GET
	*/
	$id = $_GET['id'];
	//create an sql handler
	$sqlHandler = new mysqli(SQL_SERVER,SQL_USERNAME,SQL_PASSWORD,SQL_DATABASE);
	//check connection
	if ($sqlHandler->connect_error){
		die("Connection to SQL server failed:". $sqlHandler->connect_error);
	}
	//update ip address
	$query = 'SELECT ip FROM Arms WHERE id="'.$id.'"';
	$result = $sqlHandler->query($query);
	if($result)
	{
		$row = $result->fetch_assoc();
		$ip = $row['ip'];
		echo $ip;
	}
	else
	{
		echo 0;
	} 
	//close connection
	$sqlHandler->close();
?>