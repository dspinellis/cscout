-- Create a function and macro call graph:

SELECT source.name AS CallingFunction, dest.name AS CalledFunction
  FROM fcalls
  INNER JOIN functions AS source ON fcalls.sourceid = source.id
  INNER JOIN functions AS dest ON fcalls.destid = dest.id
  ORDER BY CallingFunction, CalledFunction;
