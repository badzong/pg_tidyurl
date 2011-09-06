pg_tidyurl
==========

pg_tidyurl is a PostgreSQL extension function that optimizes strings for use in
urls. It replaces western UTF-8 characters with their ASCII equivalents and
removes or encodes special characters.


Install
-------

    sh> make
    sh> cp pg_tidyurl.so /usr/lib/postgresql/9.0/lib/
    sh> psql dbname
	
    dbname=# \i tidyurl.sql


Example
-------

    SELECT tidyurl('äüö') AS tidy;
    

     tidy
    ------
     auo
