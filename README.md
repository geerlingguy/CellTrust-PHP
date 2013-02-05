# CellTrust-PHP

CellTrust PHP was originally written to integrate CellTrust's SMS messaging API
for [Flocknote](http://www.flocknote.com) by Jeff Geerling.

CellTrust has a REST API that is used for sending and receiving messages, but I
couldn't find any helpful PHP documentation or pre-existing libraries, so I
wrote this class.

## Usage

This code only implements bits of the CellTrust public messaging API (and not
the private messaging API, since I don't need to use it). Sending and receiving
messages is a simple affair:

### Sending a message

```php
// To send a message, create an instance of the class with your username and
// password, then set your message/keyword parameters, and send the message.
$sms = new CellTrust('myusername', 'mypassword');
$sms->setKeyword("mykeyword");
$sms->setShortcode(00000);
$sms->setMessage("My message here.");
$sms->setPhoneNumber("13145556666");
$sms->setPhoneCarrier(2);
$sms->sendSMS();
```

### Receiving a message

```php
// To receive a message, get the data delivered to your server, create an
// instance of the class with your username and password, then parse the
// data from CellTrust.
$input = file_get_contents('php://input'); // Get CellTrust request.
$sms = new CellTrust('myusername', 'mypassword');
$sms->parseIncomingSMS($input);
// Use getReceivedData() to return entire array of parameters received from
// CellTrust.
$received_data = $sms->getReceivedData();
// You can also retrieve received data granularly.
$message = $sms->getData(); // getMessage() and getData() may be different.
```

## License

CellTrust-PHP is licensed under the MIT (Expat) license. See included LICENSE.md.
