<?php

$server = new UServer();

$server->get('/', function (HttpRequest $req, HttpResponse $res) {
    $res->setHeader('Content-Type', 'application/json');

    return json_encode(['hello' => 'world']);
});

$server->get('/hello/:name', function (HttpRequest $req, HttpResponse $res) {
    $res->setHeader('Content-Type', 'application/json');

    return json_encode(['hello' => $req->getParameter('name')]);
});

$server->listen(3000)->run();
