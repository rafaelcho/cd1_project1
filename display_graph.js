/*
        Raphael Cho, Senior of CSE Department, Sogang University.
        Creation Date: 2019-03-31-Sunday.
*/

var express = require('express');       // express framework.
var app = express();                    // create a server.
var fs = require('fs');                 // file system module.
mysql = require('mysql');               // extract a module.

var db_config = require('./config/db-config.json');
var connection = mysql.createConnection({
    host: db_config.host,
    user: db_config.user,
    password: db_config.password,
    database: db_config.database
});                                     // connect to the DB.
connection.connect();

app.get('/data', function (req, res) {
    console.log(req.query);

    var qstr = 'select * from sensors';
    connection.query(qstr, function(err, rows, cols) {
      if (err) {
          throw err;
        res.send('query error: ' + qstr);
        return;
      }

      console.log("Got ", + rows.length + " records");
      var html = "['time', 'value']"
      var last;
      for (var i=0; i< rows.length; i++) {
         html += ",[" + JSON.stringify(rows[i].time.toString()) + "," + rows[i].value + "]";
         if (i == rows.length - 1) last = i;
      }

      fs.readFile("Graph.html",'utf8', function (err, data) {
        if (err) console.log("file error" + err);

        data = data.replace("<%DATADATA%>", html);
        data += "<br>" + "Developer: Y. H. Cho" + "<br><br>";
        data += "Start data: " + rows[0].time + "<br>";
        data += "Last data: " + rows[last].time + "<br><br>";
        data += "node.js source code url - https://github.com/rafaelcho/cd1_project1/blob/master/display_graph.js"+"<br>";
        data += "arduino source code url - https://github.com/rafaelcho/cd1_project1/blob/master/tempSensing_thingspeakTrans_awsTrans.ino" + "<br>";

        res.send(data);
    });
  });
});

var server = app.listen(8082, function () {
  console.log('listening at port 8082!')
});
