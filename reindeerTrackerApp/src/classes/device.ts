import {Log} from "./log";

export interface Device {
  deviceKey: string,
  birthYear: number,
  imageUrl: string,
  name: string,
  activationDate: string,
  gender: string,
  logs: [Log],
  isAlive: boolean,
  activated: boolean,
  userIDs: [String],
  lastLog: Log
}
