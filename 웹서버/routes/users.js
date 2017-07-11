var express = require('express');
var router = express.Router();

//   MySQL 로드
var mysql = require('mysql');
var pool = mysql.createPool({
  //한번에 데이터베이스에 접속하는 사람수 제한
  connectionLimit: 5,
  host: 'localhost',
  user: 'root',
  database: 'Pee',
  password: '12345'
});

router.get('/:user_id', function(req, res, next) {
    pool.getConnection(function (err, connection) {
        // Use the connection
        id=req.params.user_id;
        var sqlForSelectList = "SELECT * FROM User WHERE id="+id;
        connection.query(sqlForSelectList, function (err, rows) {
            if (err) console.error("err : " + err);
            console.log("rows : " + JSON.stringify(rows));
            var userdata=rows;
            res.send(userdata);
            connection.release();

            // Don't use the connection here, it has been returned to the pool.
        });
    });
});

router.get('/:user_id/data', function(req, res, next) {
    pool.getConnection(function (err, connection) {
        // Use the connection
        id=req.params.user_id;
        var query= "SELECT * FROM Data WHERE user_id="+id;
        connection.query(query,function (err, rows) {
            res.send(rows);
            connection.release();
        })
        // Don't use the connection here, it has been returned to the pool.
    });
});

router.post('/:user_id', function(req, res, next) {
	pool.getConnection(function (err, connection) {
	id=req.params.user_id;
	time=JSON.stringify(req.body.time);
    var sqlquery="INSERT INTO Data(user_id, time) VALUES(" + id + ", " + time + ")";
	//var sqlquery="UPDATE User SET count="+ count +" WHERE id="+id;
	connection.query(sqlquery, function(err,rows){
	    if(err) console.error("err : " + err);
	    console.log("rows : " + JSON.stringify(rows));
        var data=rows;
        
        var query = "SELECT * FROM User WHERE id="+id;
        connection.query(query, function (err, rows) {
            var count=JSON.stringify(rows[0].count);
            count=count*1+1;
            var q="UPDATE User SET count="+ count + " WHERE id=" + id;
            connection.query(q, function (err, rows) {
                res.send("Ok");
                connection.release();
            })
        })
	});
});
})
module.exports = router;
