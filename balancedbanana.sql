-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Schema mydb
-- -----------------------------------------------------
-- -----------------------------------------------------
-- Schema balancedbanana
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema balancedbanana
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `balancedbanana` DEFAULT CHARACTER SET latin1 ;
USE `balancedbanana` ;

-- -----------------------------------------------------
-- Table `balancedbanana`.`allocated_resources`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `balancedbanana`.`allocated_resources` ;

CREATE TABLE IF NOT EXISTS `balancedbanana`.`allocated_resources` (
  `id` TINYINT(3) UNSIGNED NOT NULL AUTO_INCREMENT,
  `space` BIGINT(10) UNSIGNED NOT NULL,
  `ram` BIGINT(10) UNSIGNED NOT NULL,
  `cores` BIGINT(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `id_UNIQUE` (`id` ASC) VISIBLE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = latin1;


-- -----------------------------------------------------
-- Table `balancedbanana`.`job_results`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `balancedbanana`.`job_results` ;

CREATE TABLE IF NOT EXISTS `balancedbanana`.`job_results` (
  `id` TINYINT(3) UNSIGNED NOT NULL AUTO_INCREMENT,
  `stdout` MEDIUMTEXT NOT NULL,
  `exit_code` TINYINT(3) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `id_UNIQUE` (`id` ASC) VISIBLE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = latin1;


-- -----------------------------------------------------
-- Table `balancedbanana`.`users`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `balancedbanana`.`users` ;

CREATE TABLE IF NOT EXISTS `balancedbanana`.`users` (
  `id` TINYINT(2) UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(45) NOT NULL,
  `email` VARCHAR(60) NOT NULL,
  `key` VARCHAR(100) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `key_UNIQUE` (`key` ASC) VISIBLE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = latin1;


-- -----------------------------------------------------
-- Table `balancedbanana`.`workers`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `balancedbanana`.`workers` ;

CREATE TABLE IF NOT EXISTS `balancedbanana`.`workers` (
  `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ram` BIGINT(10) UNSIGNED NULL DEFAULT NULL,
  `cores` BIGINT(10) UNSIGNED NULL DEFAULT NULL,
  `space` BIGINT(10) UNSIGNED NULL DEFAULT NULL,
  `address` VARCHAR(255) NULL DEFAULT NULL,
  `public_key` VARCHAR(255) NULL DEFAULT NULL,
  `name` VARCHAR(45) NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `id_UNIQUE` (`id` ASC) VISIBLE,
  UNIQUE INDEX `public_key_UNIQUE` (`public_key` ASC) VISIBLE,
  UNIQUE INDEX `address_UNIQUE` (`address` ASC) VISIBLE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = latin1;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
