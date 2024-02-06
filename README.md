# REST-Web-Client
--

In this assignment, I implemented a client-server application capable of processing various commands aimed at communicating with a server on the internet. The primary tools used were the REST API. I utilized the construction of different functions implemented through the HTTP application protocol to post certain books or to display, from the server's database, certain books.

Implemented Commands:

- register: This command adds a new account to the server's database by constructing a POST function to the URL "/api/v1/tema/auth/register" with a JSON object containing the user's "username" and "password".

- login: Implementation is similar to the register command, with the difference being the URL "/api/v1/tema/auth/login". Within this command, I implemented various error messages based on the analysis of error messages received from the server in case the data is not sent correctly. Another error message occurs if attempting to connect to the server with an existing cookie. The program will display the received cookie from the server for this account if everything goes well.

- enter_library: Within this command, I used a GET function to access the library, with the URL "/api/v1/tema/library/access". The command will display an error message if the cookie does not exist in the program and will return a valid token for the current account if everything goes well.

- get_books: For this command's implementation, I needed to construct a GET REST command with the URL "/api/v1/tema/library/books". I included the cookie and token received from previous commands in the message body. In case there is no token in the system, an error message is displayed. The program will display a list of JSON objects with information about each book in the system if everything goes well.

- get_book/delete_book: In implementing these two functions, I used almost the same approach, with the only difference being the REST command sent to the server, either GET or DELETE. In both commands, I handled the situation where a non-integer "id" is entered and displayed an appropriate message. Additionally, if there is no token in the system, an error message is displayed. Another handled situation is when the id is not in the server's database. For the "get" function, it will display a book with all the corresponding data, and for the "delete" function, it will display a message if everything goes well.

- add_book: For this command, I implemented a POST command. If "\n" is entered as input, an error message is displayed, indicating that the user attempted to skip completing that field entirely. If a string is entered for the number of pages, an error message is displayed. If the token is NULL, an error message is displayed. If everything goes well, a confirmation message is displayed, and the book is added to the system.

- logout: For this command, a GET message is sent to the server. If the cookie is NULL, an error message is displayed. If everything goes well, the user should be disconnected from the server, and all data in the system will be initialized with NULL.

- exit: The exit command exits the user from the entire program.

Specifications:

I used the skeleton from laboratory 10.
I connected the program to the server for each command using the "open_connection()" function because I observed that I lose connection to the server without a concrete explanation.
The role of the "printOutPutCommand();" function is to display the results/errors for each executed command. Additionally, the function initializes the "cookie" and "token" variables when the "login" and "enter_library" commands are executed, respectively.
For parsing JSON data, I used the "parson.c" library recommended in the assignment.
