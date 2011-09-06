pg_tidyurl
==========

pg_tidyurl is a PostgreSQL extension function that optimizes strings for urls
by replaces mostly western UTF-8 characters with their ASCII equivalents and
removes or encodes special characters.


Install
-------

    make


Example
-------

    ```sql
    SELECT tidyurl('äüö') AS tidy;
    
     tidy
    ------
     auo
    ```
    
