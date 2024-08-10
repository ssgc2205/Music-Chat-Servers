The variable names were chosen to align semantically with their respective tasks and comments were mentioned to make the code more readable.
-> Implemented a simple HTTP server which supports GET and POST along with common MIME types.
-> Webserver executable takes two command line arguments the first one being port number and second one being root directory.
-> If a requested source is not available in the directory then it displays 404.html file.
-> And if no source is requested then it displays index.html.
-> Maximum number of requests that can be listened at time is 10.
-> When submitted some text the server will count number of characters, number of words and number of sentences in the text.
-> We assumed that something is considered as a sentence when '.' or '!' is used.
-> A single letter when between two spaces is also considered a word.
-> Number of characters excluded spaces and nextline('\n').