/**
 * Created by Minye0b on 2016-11-24.
 */
var express = require('express');
var router = express.Router();

//   MySQL 로드
var mysql = require('mysql');
var pool = mysql.createPool({
    //한번에 데이터베이스에 접속하는 사람수 제한
    connectionLimit: 5,
    host: 'localhost',
    user: 'root',
    database: 'Auction',
    password: '12345'
});

/* GET users listing. */
router.get('/', function(req, res, next) {
    pool.getConnection(function (err, connection) {
        // Use the connection
        //date_format은 파라미터 앞의 값, 즉 날짜를 파라미터 뒤에 값의 형태로 바꾸어준다.
        //hit는 해당
        var sqlForSelectList = "SELECT idx, creator_id, title, date_format(modidate,'%Y-%m-%d %H:%i:%s') modidate, hit FROM board";
        connection.query(sqlForSelectList, function (err, rows) {
            if (err) console.error("err : " + err);
            console.log("rows : " + JSON.stringify(rows));

            res.render('list', {title: ' 게시판 전체 글 조회', rows: rows});
            connection.release();

            // Don't use the connection here, it has been returned to the pool.
        });
    });
});

router.post('/result', function(req,res,next){

    var title = req.body.search;
    console.log(title);

    pool.getConnection(function (err, connection) {
        // Use the connection
        if (err) console.error("err : " + err);
        connection.query('SELECT * FROM board where title like "%' + title + '%"', function (err, rows) {
            console.log("rows : " + JSON.stringify(rows));

            res.render('page', {title: '검색결과', rows: rows});
            connection.release();

            // Don't use the connection here, it has been returned to the pool.
        });
    });
});

router.get('/list/:page', function(req,res,next){

    id=req.params.page;
    console.log(id);
    pool.getConnection(function (err, connection) {
        // Use the connection
        if (err) console.error("err : " + err);
        connection.query('SELECT * FROM board where idx like "%' + id + '%"', function (err, rows) {
            console.log("rows : " + JSON.stringify(rows));

            res.render('page', {title: '검색결과', rows: rows});
            connection.release();
        })
    });
});

router.get('/write', function(req,res,next){
    res.render('write2',{title : "게시판 글 쓰기"});
});

router.post('/write', function(req,res,next){

    var creator_id = req.body.Seller_id;
    var title = req.body.Title;
    var content = req.body.Content;
    var datas = [creator_id,title,content];

    pool.getConnection(function (err, connection)
    {
        // Use the connection
        var sqlForInsertBoard = "insert into Auction(Seller_id, Title, Content) values(?,?,?)";
        //values에 물음표로 표시 되있는 값은 datas에 받아 하나씩 ?에 매칭되어 값이 들어간다
        //물음표를 주지 않고 값을 직접 입력해서 줄수도 있지만 query문을 간단하고 가독성좋게 작성하기 위해 ?를 쓰고 받을 데이터를 ,로 파라미터로 넘겨
        //주는것이 더욱 편하다
        connection.query(sqlForInsertBoard,datas, function (err, rows) {
            if (err) console.error("err : " + err);
            console.log("rows : " + JSON.stringify(rows));

            res.redirect('/board');
            connection.release();

        });
    });

});

module.exports = router;