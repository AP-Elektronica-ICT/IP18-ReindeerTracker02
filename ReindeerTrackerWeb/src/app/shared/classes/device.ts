import {Log} from "./log";

export interface Device {
  deviceKey: string,
  gender: string,
  imageUrl: string,
  birthyear: string,
  name: string,
  logs: Log[],
  isAlive: boolean,
  activated: boolean,
  userIDs: string[],
  lastLog: Log
}
