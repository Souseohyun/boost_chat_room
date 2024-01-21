-- MySQL dump 10.13  Distrib 8.0.35, for Linux (x86_64)
--
-- Host: localhost    Database: UserInfo
-- ------------------------------------------------------
-- Server version	8.0.35-0ubuntu0.22.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `t_user_relationship`
--

DROP TABLE IF EXISTS `t_user_relationship`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `t_user_relationship` (
  `f_id` bigint NOT NULL AUTO_INCREMENT COMMENT '自增ID',
  `f_user_id1` bigint NOT NULL COMMENT '第一个用户id',
  `f_user_id2` bigint NOT NULL COMMENT '第二个用户id',
  `f_user1_teamname` varchar(32) NOT NULL DEFAULT '我的好友' COMMENT '用户2在用户1的好友分组名称',
  `f_user1_markname` varchar(32) DEFAULT NULL COMMENT '用户2在用户1的备注名称',
  `f_user2_teamname` varchar(32) NOT NULL DEFAULT '我的好友' COMMENT '用户1在用户2的好友分组名称',
  `f_user2_markname` varchar(32) DEFAULT NULL COMMENT '用户1在用户2的备注名称',
  `f_update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
  `f_remark` varchar(64) DEFAULT NULL COMMENT '备注',
  PRIMARY KEY (`f_id`),
  KEY `f_id` (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb3;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_user_relationship`
--

LOCK TABLES `t_user_relationship` WRITE;
/*!40000 ALTER TABLE `t_user_relationship` DISABLE KEYS */;
INSERT INTO `t_user_relationship` VALUES (1,1003,1004,'我的好友','曹楚闲','我的好友','刘倩','2023-12-20 07:56:37',NULL),(2,1001,1003,'我的好友','Root','我的好友','曹楚闲','2023-12-20 08:10:38',NULL),(3,1003,1005,'我的好友','楚闲','我的好友','老三','2024-01-07 03:00:00',NULL);
/*!40000 ALTER TABLE `t_user_relationship` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-01-18 17:02:22
