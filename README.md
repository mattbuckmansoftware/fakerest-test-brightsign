# fakerest-test-brightsign

## Install Prerequisites
```
sudo apt-get install -y build-essential curl libcurl4-openssl-dev libsimdjson-dev
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
The contents of invalid lines does not get stored (in logs, etc.), since that could be a way for a bad actor to emplace malicious code onto a device for future execution (either in a multi-step exploit or when the logs are read later). Likewise, the contents of valid lines is not logged, since that would be bad practice (if this data were actually sensitive).

## Data Format (IN)
Input data (which will be retrieved from the given endpoint) is an array of user objects. Each new line is treated as a standalone JSON object. If that line is invalid, the data on that line will be ignored.

## Data Format (OUT)
Unless options are used to specify only certain fields to be shown, the output will be a JSON object containing the following keys: `"city_data"`, `"most_common_name"`, and `"most_common_hobby"`.
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
		},
		{
			"city": "Honolulu",
			"data": {
				"average_age": 58.7605,
				"average_friend_count": 4.00462,
				"most_friends_count": 6
			}
		},
		{
			"city": "Sedona",
			"data": {
				"average_age": 58.6583,
				"average_friend_count": 4.00025,
				"most_friends_count": 6
			}
		},
		{
			"city": "Chicago",
			"data": {
				"average_age": 58.1431,
				"average_friend_count": 4.03008,
				"most_friends_count": 6
			}
		},
		{
			"city": "San Francisco",
			"data": {
				"average_age": 58.153,
				"average_friend_count": 3.98254,
				"most_friends_count": 6
			}
		},
		{
			"city": "Washington",
			"data": {
				"average_age": 58.4237,
				"average_friend_count": 3.98838,
				"most_friends_count": 6
			}
		},
		{
			"city": "St. Louis",
			"data": {
				"average_age": 58.5784,
				"average_friend_count": 3.98769,
				"most_friends_count": 6
			}
		},
		{
			"city": "Seattle",
			"data": {
				"average_age": 58.6795,
				"average_friend_count": 3.97279,
				"most_friends_count": 6
			}
		},
		{
			"city": "Portland",
			"data": {
				"average_age": 58.5875,
				"average_friend_count": 4.00995,
				"most_friends_count": 6
			}
		},
		{
			"city": "Austin",
			"data": {
				"average_age": 58.4033,
				"average_friend_count": 3.9858,
				"most_friends_count": 6
			}
		},
		{
			"city": "Palm Springs",
			"data": {
				"average_age": 58.3151,
				"average_friend_count": 4.0005,
				"most_friends_count": 6
			}
		},
		{
			"city": "Los Angeles",
			"data": {
				"average_age": 58.3727,
				"average_friend_count": 4.00914,
				"most_friends_count": 6
			}
		},
		{
			"city": "Nashville",
			"data": {
				"average_age": 58.048,
				"average_friend_count": 3.98408,
				"most_friends_count": 6
			}
		},
		{
			"city": "Charleston",
			"data": {
				"average_age": 58.9596,
				"average_friend_count": 3.95647,
				"most_friends_count": 6
			}
		},
		{
			"city": "Lahaina",
			"data": {
				"average_age": 58.2591,
				"average_friend_count": 4.00577,
				"most_friends_count": 6
			}
		},
		{
			"city": "Saint Augustine",
			"data": {
				"average_age": 58.2848,
				"average_friend_count": 3.96721,
				"most_friends_count": 6
			}
		},
		{
			"city": "New Orleans",
			"data": {
				"average_age": 58.8876,
				"average_friend_count": 4.00546,
				"most_friends_count": 6
			}
		},
		{
			"city": "Las Vegas",
			"data": {
				"average_age": 58.338,
				"average_friend_count": 3.99563,
				"most_friends_count": 6
			}
		},
		{
			"city": "Branson",
			"data": {
				"average_age": 58.8979,
				"average_friend_count": 4.01981,
				"most_friends_count": 6
			}
		},
		{
			"city": "San Antonio",
			"data": {
				"average_age": 58.2133,
				"average_friend_count": 3.9944,
				"most_friends_count": 6
			}
		},
		{
			"city": "Miami Beach",
			"data": {
				"average_age": 58.0056,
				"average_friend_count": 4.03512,
				"most_friends_count": 6
			}
		},
		{
			"city": "Boston",
			"data": {
				"average_age": 58.2029,
				"average_friend_count": 4.01298,
				"most_friends_count": 6
			}
		},
		{
			"city": "Orlando",
			"data": {
				"average_age": 58.5398,
				"average_friend_count": 3.99108,
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