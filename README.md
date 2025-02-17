# fakerest-test-brightsign

## Install Prerequisites
```
sudo apt-get install -y build-essential curl libcurl4-openssl-dev libsimdjson-dev pkg-config libgtest-dev
```

## Build and Run
```
make
./fakerest_test_brightsign http://test.brightsign.io:3000
```

Options are available to customize the data fields output by the program. However, all data fields are printed if no options are used.
Use the `-h` flag to learn about the available options or see below.
```
Usage: program <endpoint> [options; defaults to "all"]
Options:
  -h             Show this [H]elp message
  -y             Retrieve [Y]ears of life, average per city
  -f             Retrieve [F]riend count, average per city
  -m             Retrieve user with [M]ost friend count, per city
  -n             Retrieve most common first [N]ame, across all cities
  -r             Retrieve most common [R]ecreational hobby, across all friends in all cities
```

## Build Tests and Run
```
make test
./run_tests
```

## Security
### Encryption
If the data being pulled were sensitive, it would be best to use `https` or, even, manual encryption/decryption.

### Parsing
The `simdjson` JSON parsing library was used, rather than implementing a custom JSON parser. This adds security by using a well-tested utility. Although, it has a downside, in that any vulnerabilities that do exist will be common-knowledge to bad actors.

### Logging
The contents of invalid lines does not get stored (in logs, etc.), since that could be a way for a bad actor to emplace malicious code onto a device for future execution (either in a multi-step exploit or when the logs are read later). Likewise, the contents of valid lines is not logged, since that would be potential data leak a bad actor could use in the future (if this data were actually sensitive).

## Data Format (IN)
Input data (which will be retrieved from the given endpoint) is an array of user objects. Each new line is treated as a standalone JSON object. If that line is invalid, the data on that line will be ignored.
### Example Input
The input is JSON objects on multiple lines, as shown below.
```
{"id":0,"name":"Elijah","city":"Austin","age":78,"friends":[{"name":"Robert","hobbies":["Traveling","Video Games"]}]}
{"id":1,"name":"Noah","city":"Boston","age":97,"friends":[{"name":"Oliver","hobbies":["Watching Sports"]},{"name":"Olivia","hobbies":["Running","Music"]}]}
```

## Data Format (OUT)
Unless options are used to specify that only certain fields should be shown, the output will be a JSON object containing the following keys: `"city_data"`, `"most_common_name"`, and `"most_common_hobby"`.
### city_data
The value of `"city_data"` is an array containing JSON objects. The keys in the objects are `"city"` (the city name) and `"data"` (containing keys/values for `"average_age"` (float), `"average_friend_count"` (float), and `"most_friends_count"` (int)).

### most_common_name
The value of `"most_common_name"` is a JSON object containing keys/values for `"name"` (string) and `"count"` (int). The `name` is the human name that was most prevalent in the data (does not consider friend names in counts, since they don't have unique IDs). The `count` is the number of times that name occurs.

### most_common_hobby
The value of `"most_common_hobby"` is a JSON object containing key/values for `"hobby"` (string) and `"count"` (int). The `hobby` is the hobby that was most prevalent in the friends' data. The `count` is the number of times that hobby occurs.

### Example Output
The output is a single JSON object, as shown below.
```
{
	"city_data": [
		{
			"city": "New York City",
			"data": {
				"average_age": 58.5097,
				"average_friend_count": 4.00621,
				"most_friends_count": 6
			}
		},
		{
			"city": "Savannah",
			"data": {
				"average_age": 58.6574,
				"average_friend_count": 3.99564,
				"most_friends_count": 6
			}
		},
		{
			"city": "San Diego",
			"data": {
				"average_age": 58.7286,
				"average_friend_count": 4.01215,
				"most_friends_count": 6
			}
		}
	],
	"most_common_name": {
		"name": "Jack",
		"count": 5858
	},
	"most_common_hobby": {
		"hobby": "Playing Cards",
		"count": 53279
	}
}
```

## Error Handling
As already mentioned, invalid lines of data will be thrown out (and the event will be logged to /tmp/fakerestlog<yyyymmdd>.log). However, if there's an error accessing the data in the first place, the program will attempt to reach the endpoint up to 3 times with 5 second pauses between attempts. After that, an error return code (2) will be yielded and execution will halt.