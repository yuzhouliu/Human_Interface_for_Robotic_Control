<?php
	/*
	* This file update the ip address for the robot arm
	* Return : 1 - Success; 0 - Failure
	* Example: update_ip.php?id=1&ip=128.0.1:5000
	*/
	include "system.inc";
	/*
	* Retrieve data from GET
	*/
	$id = $_GET['id'];
	$ip = $_GET['ip'];
	//create an sql handler
	$sqlHandler = new mysqli(SQL_SERVER,SQL_USERNAME,SQL_PASSWORD,SQL_DATABASE);
	//check connection
	if ($sqlHandler->connect_error){
		die("Connection to SQL server failed:". $sqlHandler->connect_error);
	}
	//update ip address
	$query = 'UPDATE Arms SET '.'ip="'.$ip.'"'.' WHERE id='.$id;
	$result = $sqlHandler->query($query);
	if($result) echo 1;
	else echo 0;
	//close connection
	$sqlHandler->close();
?>