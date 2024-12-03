-- Find identifiers of a given type (typedefs, in this case)

SELECT name FROM ids WHERE ids.typedef ORDER BY name;
