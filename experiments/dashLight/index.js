/* Local mosquitto without password */
const dash_button = require("node-dash-button");
const { exec } = require("child_process");

const BARILLA_DASH_ADDRESS = "50:f5:da:a0:57:e6";

const dash = dash_button(BARILLA_DASH_ADDRESS);

dash.on("detected", () => {
  const command =
    "mosquitto_pub -h localhost -t /house/light/command -m toggleLight";

  console.log(
    `Sending MQTT command: ${"mosquitto_pub -h localhost -t /house/light/command -m toggleLight"}`
  );
  exec(command);
});
