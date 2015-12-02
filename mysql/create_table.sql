CREATE TABLE IF NOT EXISTS `variable` (
  `fecha` datetime NOT NULL,
  `id` varchar(30) NOT NULL,
  `nombre` varchar(100) NOT NULL,
  `valor` float NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;