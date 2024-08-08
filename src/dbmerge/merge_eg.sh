(
  (
    create_empty 1.db
    merge 1.db file-0001.db
    merge 1.db file-0002.db
  ) &
  pid1=$!

  (
    create_empty 3.db
    merge 3.db file-0003.db
    merge 3.db file-0004.db
  ) &
  pid2=$!

  wait $pid1
  wait $pid2
  merge 1.db 3.db
) &
