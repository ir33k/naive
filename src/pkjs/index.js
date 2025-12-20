var Clay = require('pebble-clay')

var vibs = [
  { "value": 0, "label": "None" },
  { "value": 1, "label": "Short" },
  { "value": 2, "label": "Long" },
  { "value": 3, "label": "Double" }
]

new Clay([
  {
    "type": "heading",
    "defaultValue": "Naive"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "BG",
        "label": "Background",
        "allowGray": false,
        "defaultValue": "000000"
      },
      {
        "type": "color",
        "messageKey": "FG",
        "label": "Foreground",
        "allowGray": false,
        "defaultValue": "ffffff"
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Text lines"
      },
      {
        "type": "text",
        "defaultValue": "Each text line is defined with format string.  Format string can contain regular text, numbers and special characters.  But also special placeholders that are turned into date, time, battery percent, icons and more.  There are 4 select menus with presets proposed for each text line but you can use any format string in any text line."
      },
      {
        "type": "text",
        "defaultValue": "Placeholders for date and time start with % and follow strftime(3) manual page.  Try few presets to learn common combinations.  To insert regular % use %%."
      },
      {
        "type": "text",
          "defaultValue": "Placeholders for Pebble values start with #. #b is battery percent, #s steps counter, #c is chronograph which value resets on Tap (shake). To insert regular # use ##."
      },
      {
        "type": "text",
        "defaultValue": "Placeholders for icons start with *. *h is heart, *q is quiet time visible when active, *c is battery charging indicator visible during charging, *b shows battery icon, *w is a warning icon that appears when Bluetooth connection is lost, *s is icon for steps, *l shows Bloob looking to the left and *r is Bloob looking to the right. To insert regular * use **."
      },
      {
        "id": "text0-select",
        "type": "select",
        "label": "Presets",
        "defaultValue": "#s*s",
        "options": [
          { "value": "",            "label": "NOTHING" },
          { "value": "#s*s", "label": "500👟" },
          { "value": "Steps #s*h",        "label": "Steps 500♥️" }
        ]
      },
      {
        "id": "text0-input",
        "type": "input",
        "messageKey": "TEXT0",
        "label": "Format",
        "defaultValue": "#b%%",
        "description": "Top left text"
      },
      {
        "id": "text1-select",
        "type": "select",
        "label": "Presets",
        "defaultValue": "*l#i*w*q",
        "options": [
          { "value": "",            "label": "NOTHING" },
          { "value": "*l#i*w*q",    "label": "Blob ⚠️🔕" },
          { "value": "*l#i",        "label": "Blob" },
          { "value": "*c*w*q",      "label": "🗲⚠️🔕" }
        ]
      },
      {
        "id": "text1-input",
        "type": "input",
        "messageKey": "TEXT1",
        "label": "Format",
        "defaultValue": "*c*s",
        "description": "Top right text"
      },
      {
        "id": "text2-select",
        "type": "select",
        "label": "Presets",
        "defaultValue": "#b%%*b*c",
        "options": [
          { "value": "",            "label": "NOTHING" },
          { "value": "#b%%*b*c",    "label": "85%🔋⚡️" },
          { "value": "#b%%*c",      "label": "85%⚡️" }
        ]
      },
      {
        "id": "text2-input",
        "type": "input",
        "messageKey": "TEXT2",
        "label": "Format",
        "defaultValue": "#s*h",
        "description": "Bottom left text"
      },
      {
        "id": "text3-select",
        "type": "select",
        "label": "Presets",
        "defaultValue": "%A %d",
        "options": [
          { "value": "",              "label": "NOTHING" },
          { "value": "%A %d",         "label": "Monday 18" },
          { "value": "%p",            "label": "AM/PM" },
          { "value": "%a %d",         "label": "Sun 18" },
          { "value": "%B %d",         "label": "November 18" },
          { "value": "%b %d",         "label": "Nov 18" },
          { "value": "%m/%d/%y",      "label": "11/18/24" },
          { "value": "%Y.%m.%d",      "label": "2022.11.18" },
          { "value": "%d.%m.%Y",      "label": "18.11.2022" },
          { "value": "%B %d,%S",      "label": "Date with seconds" }
        ]
      },
      {
        "id": "text3-input",
        "type": "input",
        "messageKey": "TEXT3",
        "label": "Format",
        "defaultValue": "%A %d",
        "description": "Bottom right text"
      },
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Vibrations"
      },
      {
        "type": "select",
        "messageKey": "BTOFF",
        "label": "Bluetooth disconnected",
        "defaultValue": 0,
        "options": vibs
      },
      {
        "type": "select",
        "messageKey": "BTON",
        "label": "Bluetooth connected",
        "defaultValue": 0,
        "options": vibs
      },
      {
        "type": "select",
        "messageKey": "HOUR",
        "label": "Hourly",
        "defaultValue": 0,
        "options": vibs
      },
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Other"
      },
      {
        "messageKey": "SHADOW",
        "type": "slider",
        "defaultValue": 6,
        "label": "Shadow strength",
        "description": "The decoration on the left edge",
        "min": 0,
        "max": 6,
        "step": 1
      },
      {
        "messageKey": "SECONDS",
        "type": "select",
        "label": "Seconds",
	"description": "First option disables seconds update; time, side and bottom texts are updated only each minute. Second option enables updates each second for side and bottom texts, use %S in their format to show seconds. Every other option enables seconds update after Tap (whist shake) for some amount of time to avoid draining battery.",
	// NOTE(Irek): Select input always return value as a string.
	// I have to do atoi() conversion in main code anyway.
        "defaultValue": "0",
        "options": [
          { "value": "0",  "label": "Disabled" },
          { "value": "-1", "label": "Enable" },
          { "value": "5",  "label": "5 seconds" },
          { "value": "10", "label": "10 seconds" },
          { "value": "15", "label": "15 seconds" },
          { "value": "30", "label": "30 seconds" },
          { "value": "60", "label": "1 min" },
          { "value": "300", "label": "5 min" }
        ]
      },
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save"
  }
], function () {
  this.on(this.EVENTS.AFTER_BUILD, function () {
    ;[
      { select: this.getItemById("text0-select"), input: this.getItemById("text0-input") },
      { select: this.getItemById("text1-select"), input: this.getItemById("text1-input") },
      { select: this.getItemById("text2-select"), input: this.getItemById("text2-input") },
      { select: this.getItemById("text3-select"), input: this.getItemById("text3-input") },
    ].forEach(function (text) {
      text.select.on("change", function () {
        text.input.set(text.select.get)
      })
    })
  }.bind(this))
})
