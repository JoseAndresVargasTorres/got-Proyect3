const express = require('express');
const app = express();

// Settings
app.set('port', process.env.PORT || 3000);

// Middlewares -> Funciones que se ejecutan antes de procesar algo
app.use(express.json());

// Routes (URL's)
app.use(require('./routes/database_got'));

// Starting the server
app.listen(app.get('port'), () => {
  console.log(`Server on port ${app.get('port')}`);
});
