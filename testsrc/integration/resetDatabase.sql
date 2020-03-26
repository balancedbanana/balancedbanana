USE `balancedbanana` ;

-- -----------------------------------------------------
-- Reset Jobs' Table
-- -----------------------------------------------------

ALTER TABLE `balancedbanana`.`jobs` CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL;
DELETE FROM jobs;
ALTER TABLE jobs CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT;

-- -----------------------------------------------------
-- Reset Workers' Table
-- -----------------------------------------------------

ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL;
DELETE FROM workers;
ALTER TABLE workers CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT;

-- -----------------------------------------------------
-- Reset Users' Table
-- -----------------------------------------------------

DELETE FROM users;

-- -----------------------------------------------------
-- Reset Allocated Resources' Table
-- -----------------------------------------------------

ALTER TABLE allocated_resources CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL;
DELETE FROM allocated_resources;
ALTER TABLE allocated_resources CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT;

-- -----------------------------------------------------
-- Reset Job Results' Table
-- -----------------------------------------------------

ALTER TABLE job_results CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL;
DELETE FROM job_results;
ALTER TABLE job_results CHANGE COLUMN `id` `id` BIGINT(10) UNSIGNED NOT NULL AUTO_INCREMENT;

