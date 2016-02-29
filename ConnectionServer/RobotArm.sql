-- phpMyAdmin SQL Dump
-- version 4.2.11
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Feb 29, 2016 at 03:51 AM
-- Server version: 5.6.21
-- PHP Version: 5.5.19

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `RobotArm`
--

-- --------------------------------------------------------

--
-- Table structure for table `Arms`
--

CREATE TABLE IF NOT EXISTS `Arms` (
`ID` int(16) unsigned NOT NULL COMMENT 'Store the ID of the robot arm',
  `IP` varchar(20) COLLATE utf8_bin DEFAULT NULL COMMENT 'Store the IP address of the robot arm'
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

--
-- Dumping data for table `Arms`
--

INSERT INTO `Arms` (`ID`, `IP`) VALUES
(1, '127.0.0.1:5000');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `Arms`
--
ALTER TABLE `Arms`
 ADD PRIMARY KEY (`ID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `Arms`
--
ALTER TABLE `Arms`
MODIFY `ID` int(16) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Store the ID of the robot arm',AUTO_INCREMENT=3;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
