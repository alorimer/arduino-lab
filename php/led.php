<!DOCTYPE html>

  <body>
    <?php

      if(isset($_POST['submit'])) {
        $file = fopen("state.txt", "w") or die("can't open file");
        $current = fgets($file);
        echo "Current state " + $current;

        switch ($current) {
          case "0":
            fwrite($file, "1");
            echo "Changed state to 1";
          case "1":
            fwrite($file, "0");
            echo "Changed state to 0";
          }

        fclose($file);
      }

    ?>

    <form action="<?=$_SERVER['PHP_SELF'];?>" method="post">
        <input type="submit" name="submit" value="Change state" />
    </form>

  </body>

</html>
