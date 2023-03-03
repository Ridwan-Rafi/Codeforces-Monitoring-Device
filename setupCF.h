

WiFiClientSecure client;

// Just the base of the URL you want to connect to
#define TEST_HOST "codeforces.com"

// OPTIONAL - The finferprint of the site you want to connect to.
#define TEST_HOST_FINGERPRINT "D1 40 5B 67 17 F5 21 9E 4D C0 52 B6 BA 96 81 0A 7F 8B 13 DA"
// The finger print will change every few months.

void makeHTTPRequest() {

  // Opening connection to server (Use 80 as port if HTTP)'
  
  if (!client.connect(TEST_HOST, 443)) {
    Serial.println(F("64->Connection failed"));
    return;
  } else Serial.println(F("67->Connection successfull"));

  // give the esp a breather
  yield();

  // Send HTTP request
  client.print(F("GET "));
  // This is the second half of a request (everything that comes after the base URL)
  client.print("/api/user.status?handle=Ridwan_Islam&from=1&count=1");  // %2C == ,

  // HTTP 1.0 is ideal as the response wont be chunked
  // But some API will return 1.1 regardless, so we need
  // to handle both.
  client.println(F(" HTTP/1.1"));

  //Headers
  client.print(F("Host: "));
  client.println(TEST_HOST);

  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  } else Serial.println(F("Request sended to host"));
  //delay(100);
  // Check HTTP status
  char http_status[32] = { 0 };
  client.readBytesUntil('\r', http_status, sizeof(http_status));
  Serial.println(http_status);
  // Check if it responded "OK" with either HTTP 1.0 or 1.1
  if (strcmp(http_status, "HTTP/1.1 200 ") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(http_status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  // For APIs that respond with HTTP/1.1 we need to remove
  // the chunked data length values at the start of the body
  //
  // peek() will look at the character, but not take it off the queue
  while (client.available() && client.peek() != '{' && client.peek() != '[') {
    char c = 0;
    client.readBytes(&c, 1);
    Serial.print(c);
    Serial.println("BAD");
  }

  // While the client is still availble read each
  // byte and print to the serial monitor
  char json_data[1000];
  int data_size = 0;
  while (client.available()) {
    char c = 0;
    client.readBytes(&c, 1);
    //Serial.print(c);
    json_data[data_size++] = c;
  }
  json_data[data_size] = '\0';

  Serial.println(json_data);

  const size_t capacity = 2 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 510;
  DynamicJsonBuffer jsonBuffer(capacity);

  const char* json = json_data;  //Json data

  JsonObject& root = jsonBuffer.parseObject(json);

  const char* status = root["status"];  // "OK"

  JsonObject& result_0 = root["result"][0];
  long result_0_id = result_0["id"];                                    // 193893475
  int result_0_contestId = result_0["contestId"];                       // 1795
  long result_0_creationTimeSeconds = result_0["creationTimeSeconds"];  // 1676563521
  int result_0_relativeTimeSeconds = result_0["relativeTimeSeconds"];   // 5421

  JsonObject& result_0_problem = result_0["problem"];
  int result_0_problem_contestId = result_0_problem["contestId"];  // 1795
  const char* result_0_problem_index = result_0_problem["index"];  // "B"
  const char* result_0_problem_name = result_0_problem["name"];    // "Ideal Point"
  const char* result_0_problem_type = result_0_problem["type"];    // "PROGRAMMING"
  int result_0_problem_rating = result_0_problem["rating"];        // 900

  JsonArray& result_0_problem_tags = result_0_problem["tags"];
  const char* result_0_problem_tags_0 = result_0_problem_tags[0];  // "brute force"
  const char* result_0_problem_tags_1 = result_0_problem_tags[1];  // "geometry"
  const char* result_0_problem_tags_2 = result_0_problem_tags[2];  // "greedy"

  JsonObject& result_0_author = result_0["author"];
  int result_0_author_contestId = result_0_author["contestId"];  // 1795

  const char* result_0_author_members_0_handle = result_0_author["members"][0]["handle"];  // "Ridwan_Islam"

  const char* result_0_author_participantType = result_0_author["participantType"];  // "CONTESTANT"
  bool result_0_author_ghost = result_0_author["ghost"];                             // false
  long result_0_author_startTimeSeconds = result_0_author["startTimeSeconds"];       // 1676558100

  const char* result_0_programmingLanguage = result_0["programmingLanguage"];  // "GNU C++17"
  const char* result_0_verdict = result_0["verdict"];                          // "WRONG_ANSWER"
  const char* result_0_testset = result_0["testset"];                          // "TESTS"
  int result_0_passedTestCount = result_0["passedTestCount"];                  // 0
  int result_0_timeConsumedMillis = result_0["timeConsumedMillis"];            // 0
  int result_0_memoryConsumedBytes = result_0["memoryConsumedBytes"];          // 0

  //.................. Json parsing end..........

  Serial.println("Json parsing successfull");
  Serial.print("status= ");
  Serial.println(status);
  Serial.print("Verdict= ");
  Serial.println(result_0_verdict);

  client.stop();
}

void setupCF() {

  if(client.connected())
  {
    Serial.println("Already connected");
  }
  else{
    Serial.println("Not connected to another client");
  }
  client.setFingerprint(TEST_HOST_FINGERPRINT);
  makeHTTPRequest();
}
