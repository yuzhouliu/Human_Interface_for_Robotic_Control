<?php
	/* This file contains all the constants for the system
	 *
	 * host: http://robotarm.net23.net
	 */

	$isTesting = 0;
	if ($isTesting)
	{
		/* Testing Variables */
		define("SQL_SERVER","localhost");
		define("SQL_DATABASE","RobotArm");
		define("SQL_USERNAME","root");
		define("SQL_PASSWORD","");
	}
	else
	{
		/* Real Variables */
		define("SQL_SERVER","mysql4.000webhost.com");
		define("SQL_DATABASE","a1700127_RB");
		define("SQL_USERNAME","a1700127_group88");
		define("SQL_PASSWORD","robotgroup888");
	}
?>