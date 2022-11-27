create database DB_GOT;
use DB_GOT;

drop table diff;
drop table archivo;
drop table commit;
drop table repositorio;

CREATE TABLE repositorio (
  id_repositorio INT PRIMARY KEY AUTO_INCREMENT,
  nombre_repositorio VARCHAR(128) NOT NULL
);

CREATE TABLE commit (
  id_commit INT PRIMARY KEY AUTO_INCREMENT,
  hash_commit VARCHAR(128) NOT NULL,
  comentario VARCHAR(128) NOT NULL,
  relacion_repositorio INT NOT NULL,
  FOREIGN KEY (relacion_repositorio) REFERENCES repositorio(id_repositorio)
);

CREATE TABLE diff (
  id_diff INT PRIMARY KEY AUTO_INCREMENT,
  nombre_archivo VARCHAR(128) NOT NULL,
  codigo_diff_anterior BLOB NOT NULL,
  codigo_diff_posterior BLOB NOT NULL,
  checksum VARCHAR(128) NOT NULL,
  relacion_commit INT NOT NULL, 
  FOREIGN KEY (relacion_commit) REFERENCES commit(id_commit)
);

CREATE TABLE archivo (
  id_archivo INT PRIMARY KEY AUTO_INCREMENT,
  nombre_archivo VARCHAR(128) NOT NULL,
  codigo_huffman BLOB NOT NULL,
  simbolo_codigo BLOB NOT NULL,
  relacion_commit INT NOT NULL, 
  FOREIGN KEY (relacion_commit) REFERENCES commit(id_commit)
);