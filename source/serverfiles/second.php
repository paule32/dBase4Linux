<?php
// 1. Autoload the SDK Package. This will include all the files and classes to your autoloader

require __DIR__  . '/vendor/autoload.php';

// 2. Provide your Secret Key. Replace the given one with your app clientId, and Secret
// https://developer.paypal.com/webapps/developer/applications/myapps

$apiContext = new \PayPal\Rest\ApiContext(
    new \PayPal\Auth\OAuthTokenCredential(
        'AVIT7a86OkR-J3mvO7ZfUzEWqPngZHSkLJ1LQQHwXJC8AvQQP-9yMiF5RiPe61Tk2YjoMzcJngFqofOM',     // ClientID
        'EOcyAwRLQrgynFQV3TPilfM4neT0W9obbnZ9O651NhjBPfbZzUG7wDD635sY7Ydd_wDKv4cfyIA5_c8J'      // ClientSecret
    )
);

$apiContext->setConfig(
  array(
    'log.LogEnabled' => true,
    'log.FileName' => 'PayPal.log',
    'log.LogLevel' => 'FINE'
  )
);

// 3. Lets try to save a credit card to Vault using Vault API mentioned here
// https://developer.paypal.com/webapps/developer/docs/api/#store-a-credit-card

$creditCard = new \PayPal\Api\CreditCard();
$creditCard->setType("visa")
    ->setNumber("4417119669820331")
    ->setExpireMonth("11")
    ->setExpireYear("2019")
    ->setCvv2("012")
    ->setFirstName("Joe")
    ->setLastName("Shopper");

// 4. Make a Create Call and Print the Card

try {
    $creditCard->create($apiContext);
    echo $creditCard;
}
catch (\PayPal\Exception\PayPalConnectionException $ex) {
    // This will print the detailed information on the exception.
    //REALLY HELPFUL FOR DEBUGGING

    echo $ex->getData();
}
