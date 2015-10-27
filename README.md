# nfc-smart-tag
Automatically exported from code.google.com/p/nfc-smart-tag

The NFC Smart tag is a self-contained device, which can push digitally signed one-time URL's to NFC equipped mobile devices through a variety of NFC protocols, including:

* Android BEAM (SNEP over LLCP)
* Android NPP
* Tag Emulation
* Felica Push

Pushing digitally signed URL's tied to physical locations allows for a variety of on-line to off-line applications, such as:
* Check-in by touch
* Loyalty coupons / stamp cards
* Access to special offers
* Authorization of a merchant action by NFC touch

This project contains:
* Full Firmware source code
* Complete Hardware designs and part lists for two form factors
* Assembly instructions
* A test server to decode secure URL's (naturally, for the default key only) plus Java source code
* Mechanical specs and graphic templates for custom labels

The following diagram gives an overview over the software stack and the supported devices. For more information about the firmware, see [Firmware](https://github.com/spac3lord/nfc-smart-tag/tree/master/firmware)

![Firmware Stack](http://nfc-smart-tag.googlecode.com/git/docs/FirmwareStack.png)
