#!/bin/csh

./s.sh
set j = 1
while ( $j <= 200 )
  ../driver_cpe >> output.txt
end
