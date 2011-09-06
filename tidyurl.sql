CREATE OR REPLACE FUNCTION tidyurl(varchar)
RETURNS varchar AS 'pg_tidyurl.so', 'tidyurl'
LANGUAGE C STRICT IMMUTABLE;
