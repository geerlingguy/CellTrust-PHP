<?php

/**
 * @file
 * CellTrust API Integration class.
 *
 * Note that much of the API is not implemented at this point, notably nothing
 * at all from the Private SMS gateway. This class was built to serve the needs
 * of Flocknote, so many parts we don't use aren't implemented or supported.
 *
 * Example usage for sending an SMS:
 * @code
 *   $sms = new CellTrust('myusername', 'mypassword');
 *   $sms->setKeyword("mykeyword");
 *   $sms->setShortcode(00000);
 *   $sms->setMessage("My message here.");
 *   $sms->setPhoneNumber("13145556666");
 *   $sms->setPhoneCarrier(2);
 *   $sms->sendSMS();
 * @endcode
 *
 * Example usage for receiving an SMS:
 * @code
 *   $input = file_get_contents('php://input'); // Get CellTrust request.
 *   $sms = new CellTrust('myusername', 'mypassword');
 *   $sms->parseIncomingSMS($input);
 *   // Use getReceivedData() to return entire array of parameters received
 *   // from CellTrust.
 *   $received_data = $sms->getReceivedData();
 *   // You can also retrieve received data granularly.
 *   $message = $sms->getData(); // getMessage() and getData() may be different.
 * @endcode
 *
 * @author Jeff Geerling
 */

class CellTrust {

  protected $username, $password;
  protected $keyword; // Referred to in SDK as 'CustomerNickname'.
  protected $shortcode;
  protected $endpoint_html = 'https://gateway.celltrust.net/TxTNotify/TxTNotify';
  protected $message;
  protected $phone_number, $phone_carrier;
  protected $is_sent_successfully = FALSE;
  protected $message_id = 0;
  protected $error_message = FALSE;

  // Incoming message variables.
  protected $receivedData = array();
  protected $data;

  /**
   * Creates a new CellTrust instance.
   */
  public function __construct($username = '', $password = '') {
    $this->username = $username;
    $this->password = $password;
  }

  /**
   * Send an SMS message.
   */
  public function sendSMS() {
    // Set up fields for the API call.
    $fields = array(
      'PhoneDestination' => $this->phone_number,
      'Username' => $this->username,
      'Password' => $this->password,
      'Message' => $this->message,
      'XMLResponse' => 'true',
    );

    // Add carrierId if a phone_carrier is present.
    if (!empty($this->phone_carrier)) {
      $fields['carrierId'] = $this->phone_carrier;
    }

    // Add CustomerNickname if a keyword is present.
    if (!empty($this->keyword)) {
      $fields['CustomerNickname'] = $this->keyword;
    }

    // Add Shortcode if a shortcode is present.
    if (!empty($this->shortcode)) {
      $fields['Shortcode'] = $this->shortcode;
    }

    // Build the http query.
    $query = http_build_query($fields, '', '&');
    $url = $this->endpoint_html . '?' . $query;

    // Send the request using cURL.
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
    $result = curl_exec ($ch);
    curl_close($ch);

    // If the message was accepted, store the message ID and set the sent
    // successfully property to TRUE.
    $result = new SimpleXMLElement($result);
    if (!empty($result->MsgResponseList->MsgResponse->Status) && $result->MsgResponseList->MsgResponse->Status == 'ACCEPTED') {
      $this->message_id = (string) $result->MsgResponseList->MsgResponse->MessageId;
      $this->is_sent_successfully = TRUE;
    }
    elseif (!empty($result->Error)) {
      // Get the error and set it as the error message.
      $error_code = $result->Error->ErrorCode;
      $error_message = $result->Error->ErrorString;
      $this->error_message = $error_code . ' - ' . $error_message;
    }
  }

  /**
   * Inform caller whether this message has been sent successfully.
   */
  public function isSentSuccessfully() {
    return $this->is_sent_successfully;
  }

  /**
   * Allow caller to get an error message (FALSE if no error present).
   */
  public function getErrorMessage() {
    return $this->error_message;
  }

  /**
   * Parse an incoming message.
   */
  public function parseIncomingSMS($input) {
    // Parse query string and store values in $receivedData.
    parse_str($input, $this->receivedData);
    $this->message = $this->receivedData['Message'];
    $this->data = $this->receivedData['Data'];
    $this->keyword = $this->receivedData['CustomerNickname'];
    $this->phone_number = $this->receivedData['OriginatorAddress'];
    $this->phone_carrier = isset($this->receivedData['Carrier']) ? $this->receivedData['Carrier'] : NULL;
  }

  /**
   * Setters and getters.
   */

  // Message ID.
  public function setMessageId($id) {
    $this->message_id = $id;
  }
  public function getMessageId() {
    return $this->message_id;
  }

  // Message.
  public function setMessage($message) {
    $this->message = $message;
  }
  public function getMessage() {
    return $this->message;
  }

  // Phone number.
  public function setPhoneNumber($phone_number) {
    if (!is_numeric($phone_number)) {
      throw new Exception("Phone number must not contain non-numeric characters.");
    }
    if (strlen($phone_number) < 11) {
      throw new Exception("Phone number must be international number.");
    }
    $this->phone_number = $phone_number;
  }
  public function getPhoneNumber() {
    return $this->phone_number;
  }

  // Phone carrier.
  public function setPhoneCarrier($phone_carrier) {
    $this->phone_carrier = $phone_carrier;
  }
  public function getPhoneCarrier() {
    return $this->phone_carrier;
  }

  // Shortcode.
  public function setShortcode($shortcode) {
    $this->shortcode = $shortcode;
  }
  public function getShortcode() {
    return $this->shortcode;
  }

  // Keyword.
  public function setKeyword($keyword) {
    $this->keyword = strtolower($keyword);
  }
  public function getKeyword() {
    return $this->keyword;
  }

  // Received data.
  public function getReceivedData() {
    return $this->receivedData;
  }

  // Data.
  public function setData($data) {
    $this->data = $data;
  }
  public function getData() {
    return $this->data;
  }
}
