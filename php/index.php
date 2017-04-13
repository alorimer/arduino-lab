<!DOCTYPE HTML>

<html>

  <body>
    <?php
      if (isset($_POST['status'])) {
         $fh = fopen("ledstatus.txt", 'w') or die("can't open file");
         $on = $_POST['status'];
         fwrite($fh, $on);
         fclose($fh);

      } else {
         $fh = fopen("ledstatus.txt", 'r');
         $on = fread($fh, 1);
         fclose($fh);
      }
    ?>

    <form action="index.php" method="post">
       <label> <input type="radio" name="status" <?if ($on) {echo 'checked = "checked"';} ?> value="###1">On</label>
       <label> <input type="radio" name="status" <?if (!$on) {echo 'checked = "checked"';} ?> value="###0">Off</label>
       <input type="submit" value="Submit">
    </form>

  </body>
</html>
