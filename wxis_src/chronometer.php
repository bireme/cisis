<?php

class Chronometer {
    var $time;
    var $handle;

    /* Constructor  */
    function Chronometer($handle_name) {
        $this->time = 0;
        $this->handle = null;

        if ($handle_name != null) {
            $this->handle = fopen($handle_name, "a+");
            if ($this->handle == false) {
                echo("Chronometer/file open error\n");
                $this->handle = null;
            }
        }
    }

    /* Destructor */
    function destroy() {
        if ($this->handle) {
            $this->time = 0;
            if ($this->handle != null) {
                fclose($this->handle);
                $this->handle = null;
            }
        }
    }

    function start() {
        list($usec, $sec) = explode(" ", microtime());
        $this->time = ((float)$usec + (float)$sec);
    }

    function stop($message) {
        $before = $this->time;
        list($usec, $sec) = explode(" ", microtime());
        $this->time = ((float)$usec + (float)$sec);
        $dif = round($this->time - $before, 6);

        if (!$message) {
            $message = "";
        }
        if ($this->handle != null) {
            if (!fwrite($this->handle,
                    $message."[".round($before, 6).", "
                                .round($this->time, 6).", ".$dif."]\n")) {
                echo("stop/write file error\n");
            }
        } else {
            echo($message."[".round($before, 6).", "
                             .round($this->time, 6).", ".$dif."]\n");
        }

        return $dif;
    }
}
?>
