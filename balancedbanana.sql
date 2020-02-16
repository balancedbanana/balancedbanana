-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Schema balancedbanana
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `balancedbanana` DEFAULT CHARACTER SET utf8 ;
USE `balancedbanana` ;

-- -----------------------------------------------------
-- Table `balancedbanana`.`allocated_resources`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `balancedbanana`.`allocated_resources`
(
    `id`    BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    `space` BIGINT(10) UNSIGNED NOT NULL,
    `ram`   BIGINT(10) UNSIGNED NOT NULL,
    `cores` INT(10) UNSIGNED NOT NULL,
    PRIMARY KEY (`id`),
    UNIQUE INDEX `id_UNIQUE` (`id` ASC)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `balancedbanana`.`job_results`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `balancedbanana`.`job_results`
(
    `id`        BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    `stdout`    MEDIUMTEXT          NOT NULL,
    `exit_code` TINYINT(3)          NOT NULL,
    PRIMARY KEY (`id`),
    UNIQUE INDEX `id_UNIQUE` (`id` ASC)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `balancedbanana`.`users`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `balancedbanana`.`users`
(
    `id`    BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    `name`  VARCHAR(45)         NOT NULL,
    `email` VARCHAR(60)         NOT NULL,
    `key`   VARCHAR(100)        NOT NULL,
    PRIMARY KEY (`id`),
    UNIQUE INDEX `key_UNIQUE` (`key` ASC)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `balancedbanana`.`workers`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `balancedbanana`.`workers`
(
    `id`         BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    `ram`        BIGINT(10) UNSIGNED NULL DEFAULT NULL,
    `cores`      INT(10) UNSIGNED NULL DEFAULT NULL,
    `space`      BIGINT(10) UNSIGNED NULL DEFAULT NULL,
    `address`    VARCHAR(255)        NULL DEFAULT NULL,
    `public_key` VARCHAR(255)        NULL DEFAULT NULL,
    `name`       VARCHAR(45)         NULL DEFAULT NULL,
    PRIMARY KEY (`id`),
    UNIQUE INDEX `id_UNIQUE` (`id` ASC),
    UNIQUE INDEX `public_key_UNIQUE` (`public_key` ASC),
    UNIQUE INDEX `address_UNIQUE` (`address` ASC)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `balancedbanana`.`jobs`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `balancedbanana`.`jobs` (
    `id`            BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    `min_ram`       BIGINT(10) UNSIGNED DEFAULT NULL,
    `max_ram`       BIGINT(10) UNSIGNED DEFAULT NULL,
    `started_at`    TIMESTAMP(6) NULL DEFAULT NULL,
    `scheduled_at`  TIMESTAMP(6) NULL DEFAULT NULL,
    `finished_at`   TIMESTAMP(6) NULL DEFAULT NULL,
    `command`       VARCHAR(21844) NOT NULL,
    `image`         VARCHAR(500) NOT NULL,
    `blocking_mode` TINYINT(1) DEFAULT NULL,
    `working_dir`   VARCHAR(21844) NOT NULL,
    `allocated_id`  BIGINT(10) UNSIGNED DEFAULT NULL,
    `interruptible` TINYINT(1) DEFAULT NULL,
    `environment`   BLOB DEFAULT NULL,
    `min_cores`     INT(10) UNSIGNED DEFAULT NULL,
    `max_cores`     INT(10) UNSIGNED DEFAULT NULL,
    `priority`      INT(10) UNSIGNED DEFAULT NULL,
    `status_id`     INT(10) UNSIGNED NOT NULL DEFAULT '1',
    `user_id`       BIGINT(10) UNSIGNED NOT NULL,
    `worker_id`     BIGINT(10) DEFAULT NULL,
    `email`         VARCHAR(255) NOT NULL,
    PRIMARY KEY (`id`),
    UNIQUE KEY `id_UNIQUE` (`id`),
    UNIQUE KEY `allocated_id_UNIQUE` (`allocated_id`),
    KEY `user_id_idx` (`user_id`),
    CONSTRAINT `user_id` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
