<?php

  // get time
  $UTC_timestamp        = strtotime("now") + $time_correction;
  $local_timezone       = date("Z", $UTC_timestamp);
  $local_timestamp      = $UTC_timestamp + $local_timezone;

  // daylight saving
  $Melbourne = 10;
  $DAYLIGHT_SAVING = ($local_timezone / (60*60)) - $Melbourne;

  // parse and output
  $time_str = date("D d-m-Y H:i:s T", $UTC_timestamp);
  echo $time_str;

 ?>
