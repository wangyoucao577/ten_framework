import socket
import time


def is_app_started(ip, port, timeout=10):
    duration = 0

    while (duration < timeout):
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((ip, int(port)))
            print("The app server starts in %d seconds" % duration)

            return True, s
        except Exception as e:
            print("Can not connect to app server", e)
            pass

        duration += 1
        time.sleep(1)

    # The app server running behind the msgpack protocol will be GC when the
    # connection is closed. So we can _NOT_ shutdown the socket now, and the
    # socket should be closed after the closure of the client.
    return False, None