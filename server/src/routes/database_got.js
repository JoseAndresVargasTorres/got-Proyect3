const express = require('express');
const router = express.Router();

const mysqlConnection  = require('../database.js');
var md5 = require('md5');

// Solicitar ultimo # commit
router.get('/ultimo_commit', (req, res) => {
  const {relacion_repositorio} = req.body;
  mysqlConnection.query("SELECT id_commit " +
              "FROM commit " +
              "WHERE commit.relacion_repositorio ='"  + 
              relacion_repositorio + "' " +
              "ORDER BY id_commit DESC LIMIT 1 " , {relacion_repositorio}, (err, rows, fields) => {
    if(!err) {
      res.status(200).json({ rows });
    } else {
        res.status(500).send('Operacion fallida al solicitar ultimo_commit!');
    }
  });  
});

// Solicitar commit de un archivo
router.get('/solicitar_commit', (req, res) => {
  const {nombre_archivo} = req.body;
  mysqlConnection.query("SELECT archivo.relacion_commit " +
              "FROM archivo " +
              "WHERE archivo.nombre_archivo ='"  + 
               nombre_archivo + "' " , {nombre_archivo}, (err, rows, fields) => {
    if(rows.length > 0){
      res.status(200).json({rows});
    }else{
      res.status(200).json({"relacion_commit": -1});
    }
  });  
});

// Solicitar hash y comentarios
router.get('/hash_comentario/:id_commit', (req, res) => {
  const { id_commit } = req.params; 
  mysqlConnection.query('SELECT hash_commit, comentario FROM commit WHERE id_commit = ?', [id_commit], (err, rows, fields) => {
    if(!err) {
      res.status(200).json({rows});
    } else {
      res.status(500).send('Operacion fallida al solicitar hash_comentario!');
    }
  });
});

// Solicitar huffman
router.get('/codigo_huffman', (req, res) => {
  var { id_commit, nombre_archivo} = req.body;
  mysqlConnection.query("SELECT CONVERT(archivo.codigo_huffman USING utf8), CONVERT(archivo.simbolo_codigo USING utf8) " +
              "FROM archivo JOIN commit ON archivo.relacion_commit = commit.id_commit " +
              "WHERE commit.id_commit ='" + id_commit + "' AND archivo.nombre_archivo = '" + 
               nombre_archivo + "' " , {id_commit, nombre_archivo}, (err, rows, fields) => {
    if(!err) {
      res.status(200).json({rows});
    } else {
      res.status(500).send('Operacion fallida al solicitar huffman!');
    }
  }); 
});
 
// Volver un archivo a un commit posterior
router.get('/commit_posterior', (req, res) => {
  const { id_commit, nombre_archivo} = req.body;
  mysqlConnection.query("SELECT CONVERT(diff.codigo_diff_posterior USING utf8) " +
              "FROM diff JOIN commit ON diff.relacion_commit = commit.id_commit " +
              "WHERE commit.id_commit ='" + id_commit + "' AND diff.nombre_archivo = '" + 
               nombre_archivo + "' " , {id_commit, nombre_archivo}, (err, rows, fields) => {
    if(!err) {
      res.status(200).json({rows});
    } else {
      res.status(500).send('Operacion fallida al solicitar ir a commit posterior!');
    }
  });  
});

// Devolver un archivo a un commit anterior
router.get('/commit_anterior', (req, res) => {
  const { id_commit, nombre_archivo} = req.body;
  mysqlConnection.query("SELECT CONVERT(diff.codigo_diff_anterior USING utf8) " +
              "FROM diff JOIN commit ON diff.relacion_commit = commit.id_commit " +
              "WHERE commit.id_commit ='" + id_commit + "' AND diff.nombre_archivo = '" + 
               nombre_archivo + "' " , {id_commit, nombre_archivo}, (err, rows, fields) => {
    if(!err) {
      res.status(200).json({rows});
    } else {
      res.status(500).send('Operacion fallida al solicitar devolver commit anterior!');
    }
  });  
});

// Mostrar modificaciones de todos los archivos respecto a un commit anterior
router.get('/modificaciones_commit_anterior', (req, res) => {
  const { hash_commit } = req.body;
  mysqlConnection.query("SELECT diff.nombre_archivo " +
              "FROM diff JOIN commit ON diff.relacion_commit = commit.id_commit " +
              "WHERE commit.hash_commit ='" + hash_commit + "' " 
              , {hash_commit}, (err, rows, fields) => {
    if(!err) {
      res.status(200).json(rows);
    } else {
      res.status(500).send('Operacion fallida al solicitar modificacion commit anterior!');
    }
  });  
});

// Mostrar agregados de todos los archivos respecto a un commit anterior
router.get('/agregados_commit_anterior', (req, res) => {
  const { hash_commit } = req.body;
  mysqlConnection.query("SELECT archivo.nombre_archivo " +
              "FROM archivo JOIN commit ON archivo.relacion_commit = commit.id_commit " +
              "WHERE commit.hash_commit ='" + hash_commit + "' " 
              , {hash_commit}, (err, rows, fields) => {
    if(!err) {
      res.status(200).json(rows);
    } else {
      res.status(500).send('Operacion fallida al solicitar agregados commit anterior!');
    }
  });  
});

// Mostrar modificaciones respecto a un commit 
router.get('/modificaciones_commit', (req, res) => {
  const { id_commit, nombre_archivo} = req.body;
  mysqlConnection.query("SELECT diff.nombre_archivo " +
              "FROM diff JOIN commit ON diff.relacion_commit = commit.id_commit " +
              "WHERE commit.id_commit ='" + id_commit + "' AND diff.nombre_archivo = '" + 
               nombre_archivo + "' " , {id_commit, nombre_archivo}, (err, rows, fields) => {
    if(!err) {
      if(rows.length > 0){
        res.status(200).json({"existencia": "si"});
      }else{
        res.status(200).json({"existencia": "no"});
      }
    } else {
      res.status(500).send('Operacion fallida al solicitar modificaciones commit!');
    }
  });  
});

// Mostrar agregados respecto a un commit
router.get('/agregados_commit', (req, res) => {
  const { id_commit, nombre_archivo} = req.body;
  mysqlConnection.query("SELECT archivo.nombre_archivo " +
              "FROM archivo JOIN commit ON archivo.relacion_commit = commit.id_commit " +
              "WHERE commit.id_commit ='" + id_commit + "' AND archivo.nombre_archivo = '" + 
               nombre_archivo + "' " , {id_commit, nombre_archivo}, (err, rows, fields) => {
    if(!err) {
      if(rows.length > 0){
        res.status(200).json({"existencia": "si"});
      }else{
        res.status(200).json({"existencia": "no"});
      }
    } else {
      res.status(500).send('Operacion fallida al solicitar agregados commit!');
    }
  });  
});

// INSERT AN NEW REPOSITORY
router.post('/repositorio', (req, res) => {
  var {nombre_repositorio} = req.body;
  mysqlConnection.query('INSERT INTO repositorio SET?', {nombre_repositorio}, (err, rows, fields) => {
    if(!err) {
      res.status(200).json( {"id_repositorio": rows.insertId });
    } else {
      res.status(500).send('Operacion fallida al insertar repositorio!');
    }
  });
});

// INSERT AN NEW COMMIT
router.post('/commit', (req, res) => {
  var {hash_commit, comentario, relacion_repositorio} = req.body;
  // Crear hash
  hash_commit = md5(hash_commit);
  mysqlConnection.query('INSERT INTO commit SET?', {hash_commit, comentario, relacion_repositorio}, (err, rows, fields) => {
    if(!err) {
      res.status(200).json( {"hash_commit": hash_commit });
      console.log("Commit insertado!");
    } else {
      res.status(500).send('Operacion fallida al insertar commit!');
    }
  });
});

// INSERT AN NEW ARCHIVO
router.post('/archivo', (req, res) => {
  const {nombre_archivo, codigo_huffman, simbolo_codigo, relacion_commit} = req.body;
  mysqlConnection.query('INSERT INTO archivo SET?', {nombre_archivo, codigo_huffman, simbolo_codigo, relacion_commit}, (err, rows, fields) => {
    if(!err) {
      res.status(200).send('Archivo insertado!');
      console.log("Archivo insertado!");
    } else {
      res.status(500).send('Operacion fallida al insertar archivo!');
    }
  });
});

// INSERT AN NEW DIFF
router.post('/diff', (req, res) => {
  const {nombre_archivo, codigo_diff_anterior, codigo_diff_posterior, checksum, relacion_commit} = req.body;
  mysqlConnection.query('INSERT INTO diff SET?', {nombre_archivo, codigo_diff_anterior, codigo_diff_posterior, checksum, relacion_commit}, (err, rows, fields) => {
    if(!err) {
      res.status(200).send('Diff insertado!');
      console.log("Diff insertado!");
    } else {
      res.status(500).send('Operacion fallida al insertar diff!');
    }
  });
});

// UPDATE An campo DIFF (codigo_diff_anterior)
router.put('/:id_diff', (req, res) => {
  const { codigo_diff_anterior } = req.body;
  const { id_diff } = req.params;
  console.log(codigo_diff_anterior);
  mysqlConnection.query('UPDATE diff SET codigo_diff_anterior = ? WHERE id_diff = ?', [codigo_diff_anterior, id_diff], (err, rows, fields) => {
    if(!err) {
      res.status(200).send('Diff actualizado!');
    } else {
      res.status(500).send('Operacion fallida al actualizar campo diff!');
    }
  });
});

// DELETE An Employee
/*router.delete('/:id', (req, res) => {
  const { id } = req.params;
  mysqlConnection.query('DELETE FROM employee WHERE id = ?', [id], (err, rows, fields) => {
    if(!err) {
      res.json({status: 'Employee Deleted'});
    } else {
      console.log(err);
    }
  });
});*/

module.exports = router;
