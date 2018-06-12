
#pragma once

#define ALL_IBUS_MSG_COUNT      8
#define DATA_BUFF_SIZE          256
#define RadToEuler(a)           (a * 57.9323992963f)

typedef enum IBUS_RESULT
{
	IBUS_RESULT_RUNNING = 1,
	IBUS_RESULT_ACCEPTED = 0, /* mission accepted OK | */
	IBUS_RESULT_ERROR = -1, /* generic error / not accepting mission commands at all right now | */
	IBUS_RESULT_UNSUPPORTED = -2, /* coordinate frame is not supported | */
	IBUS_RESULT_TIMEOUT = -3,
	IBUS_RESULT_IDERROR = -4,
	IBUS_RESULT_NOBUFF = -5,
	IBUS_RESULT_MODEERROR = -6,
} IBUS_RESULT;


typedef enum MAV_CMD
{
	MAV_CMD_NAV_WAYPOINT = 16, /* Navigate to MISSION. |Hold time in decimal seconds. (ignored by fixed wing, time to stay at MISSION for rotary wing)| Acceptance radius in meters (if the sphere with this radius is hit, the MISSION counts as reached)| 0 to pass through the WP, if > 0 radius in meters to pass by WP. Positive value for clockwise orbit, negative value for counter-clockwise orbit. Allows trajectory control.| Desired yaw angle at MISSION (rotary wing)| Latitude| Longitude| Altitude|  */
	MAV_CMD_NAV_LOITER_UNLIM = 17, /* Loiter around this MISSION an unlimited amount of time |Empty| Empty| Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise| Desired yaw angle.| Latitude| Longitude| Altitude|  */
	MAV_CMD_NAV_LOITER_TURNS = 18, /* Loiter around this MISSION for X turns |Turns| Empty| Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise| Desired yaw angle.| Latitude| Longitude| Altitude|  */
	MAV_CMD_NAV_LOITER_TIME = 19, /* Loiter around this MISSION for X seconds |Seconds (decimal)| Empty| Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise| Desired yaw angle.| Latitude| Longitude| Altitude|  */
	MAV_CMD_NAV_RETURN_TO_LAUNCH = 20, /* Return to launch location |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_NAV_LAND = 21, /* Land at location |Abort Alt| Empty| Empty| Desired yaw angle| Latitude| Longitude| Altitude|  */
	MAV_CMD_NAV_TAKEOFF = 22, /* Takeoff from ground / hand |Minimum pitch (if airspeed sensor present), desired pitch without sensor| Empty| Empty| Yaw angle (if magnetometer present), ignored without magnetometer| Latitude| Longitude| Altitude|  */
	MAV_CMD_NAV_LAND_LOCAL = 23, /* Land at local position (local frame only) |Landing target number (if available)| Maximum accepted offset from desired landing position [m] - computed magnitude from spherical coordinates: d = sqrt(x^2 + y^2 + z^2), which gives the maximum accepted distance between the desired landing position and the position where the vehicle is about to land| Landing descend rate [ms^-1]| Desired yaw angle [rad]| Y-axis position [m]| X-axis position [m]| Z-axis / ground level position [m]|  */
	MAV_CMD_NAV_TAKEOFF_LOCAL = 24, /* Takeoff from local position (local frame only) |Minimum pitch (if airspeed sensor present), desired pitch without sensor [rad]| Empty| Takeoff ascend rate [ms^-1]| Yaw angle [rad] (if magnetometer or another yaw estimation source present), ignored without one of these| Y-axis position [m]| X-axis position [m]| Z-axis position [m]|  */
	MAV_CMD_NAV_FOLLOW = 25, /* Vehicle following, i.e. this waypoint represents the position of a moving vehicle |Following logic to use (e.g. loitering or sinusoidal following) - depends on specific autopilot implementation| Ground speed of vehicle to be followed| Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise| Desired yaw angle.| Latitude| Longitude| Altitude|  */
	MAV_CMD_NAV_CONTINUE_AND_CHANGE_ALT = 30, /* Continue on the current course and climb/descend to specified altitude.  When the altitude is reached continue to the next command (i.e., don't proceed to the next command until the desired altitude is reached. |Climb or Descend (0 = Neutral, command completes when within 5m of this command's altitude, 1 = Climbing, command completes when at or above this command's altitude, 2 = Descending, command completes when at or below this command's altitude. | Empty| Empty| Empty| Empty| Empty| Desired altitude in meters|  */
	MAV_CMD_NAV_LOITER_TO_ALT = 31, /* Begin loiter at the specified Latitude and Longitude.  If Lat=Lon=0, then loiter at the current position.  Don't consider the navigation command complete (don't leave loiter) until the altitude has been reached.  Additionally, if the Heading Required parameter is non-zero the  aircraft will not leave the loiter until heading toward the next waypoint.  |Heading Required (0 = False)| Radius in meters. If positive loiter clockwise, negative counter-clockwise, 0 means no change to standard loiter.| Empty| Empty| Latitude| Longitude| Altitude|  */
	MAV_CMD_NAV_ROI = 80, /* Sets the region of interest (ROI) for a sensor set or the vehicle itself. This can then be used by the vehicles control system to control the vehicle attitude and the attitude of various sensors such as cameras. |Region of intereset mode. (see MAV_ROI enum)| MISSION index/ target ID. (see MAV_ROI enum)| ROI index (allows a vehicle to manage multiple ROI's)| Empty| x the location of the fixed ROI (see MAV_FRAME)| y| z|  */
	MAV_CMD_NAV_PATHPLANNING = 81, /* Control autonomous path planning on the MAV. |0: Disable local obstacle avoidance / local path planning (without resetting map), 1: Enable local path planning, 2: Enable and reset local path planning| 0: Disable full path planning (without resetting map), 1: Enable, 2: Enable and reset map/occupancy grid, 3: Enable and reset planned route, but not occupancy grid| Empty| Yaw angle at goal, in compass degrees, [0..360]| Latitude/X of goal| Longitude/Y of goal| Altitude/Z of goal|  */
	MAV_CMD_NAV_SPLINE_WAYPOINT = 82, /* Navigate to MISSION using a spline path. |Hold time in decimal seconds. (ignored by fixed wing, time to stay at MISSION for rotary wing)| Empty| Empty| Empty| Latitude/X of goal| Longitude/Y of goal| Altitude/Z of goal|  */
	MAV_CMD_NAV_ALTITUDE_WAIT = 83, /* Mission command to wait for an altitude or downwards vertical speed. This is meant for high altitude balloon launches, allowing the aircraft to be idle until either an altitude is reached or a negative vertical speed is reached (indicating early balloon burst). The wiggle time is how often to wiggle the control surfaces to prevent them seizing up. |altitude (m)| descent speed (m/s)| Wiggle Time (s)| Empty| Empty| Empty| Empty|  */
	MAV_CMD_NAV_VTOL_TAKEOFF = 84, /* Takeoff from ground using VTOL mode |Empty| Empty| Empty| Yaw angle in degrees| Latitude| Longitude| Altitude|  */
	MAV_CMD_NAV_VTOL_LAND = 85, /* Land using VTOL mode |Empty| Empty| Empty| Yaw angle in degrees| Latitude| Longitude| Altitude|  */
	MAV_CMD_NAV_GUIDED_ENABLE = 92, /* hand control over to an external controller |On / Off (> 0.5f on)| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_NAV_LAST = 95, /* NOP - This command is only used to mark the upper limit of the NAV/ACTION commands in the enumeration |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_CONDITION_DELAY = 112, /* Delay mission state machine. |Delay in seconds (decimal)| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_CONDITION_CHANGE_ALT = 113, /* Ascend/descend at rate.  Delay mission state machine until desired altitude reached. |Descent / Ascend rate (m/s)| Empty| Empty| Empty| Empty| Empty| Finish Altitude|  */
	MAV_CMD_CONDITION_DISTANCE = 114, /* Delay mission state machine until within desired distance of next NAV point. |Distance (meters)| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_CONDITION_YAW = 115, /* Reach a certain target angle. |target angle: [0-360], 0 is north| speed during yaw change:[deg per second]| direction: negative: counter clockwise, positive: clockwise [-1,1]| relative offset or absolute angle: [ 1,0]| Empty| Empty| Empty|  */
	MAV_CMD_CONDITION_LAST = 159, /* NOP - This command is only used to mark the upper limit of the CONDITION commands in the enumeration |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_SET_MODE = 176, /* Set system mode. |Mode, as defined by ENUM MAV_MODE| Custom mode - this is system specific, please refer to the individual autopilot specifications for details.| Custom sub mode - this is system specific, please refer to the individual autopilot specifications for details.| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_JUMP = 177, /* Jump to the desired command in the mission list.  Repeat this action only the specified number of times |Sequence number| Repeat count| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_CHANGE_SPEED = 178, /* Change speed and/or throttle set points. |Speed type (0=Airspeed, 1=Ground Speed)| Speed  (m/s, -1 indicates no change)| Throttle  ( Percent, -1 indicates no change)| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_SET_HOME = 179, /* Changes the home location either to the current location or a specified location. |Use current (1=use current location, 0=use specified location)| Empty| Empty| Empty| Latitude| Longitude| Altitude|  */
	MAV_CMD_DO_SET_PARAMETER = 180, /* Set a system parameter.  Caution!  Use of this command requires knowledge of the numeric enumeration value of the parameter. |Parameter number| Parameter value| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_SET_RELAY = 181, /* Set a relay to a condition. |Relay number| Setting (1=on, 0=off, others possible depending on system hardware)| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_REPEAT_RELAY = 182, /* Cycle a relay on and off for a desired number of cyles with a desired period. |Relay number| Cycle count| Cycle time (seconds, decimal)| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_SET_SERVO = 183, /* Set a servo to a desired PWM value. |Servo number| PWM (microseconds, 1000 to 2000 typical)| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_REPEAT_SERVO = 184, /* Cycle a between its nominal setting and a desired PWM for a desired number of cycles with a desired period. |Servo number| PWM (microseconds, 1000 to 2000 typical)| Cycle count| Cycle time (seconds)| Empty| Empty| Empty|  */
	MAV_CMD_DO_FLIGHTTERMINATION = 185, /* Terminate flight immediately |Flight termination activated if > 0.5| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_LAND_START = 189, /* Mission command to perform a landing. This is used as a marker in a mission to tell the autopilot where a sequence of mission items that represents a landing starts. It may also be sent via a COMMAND_LONG to trigger a landing, in which case the nearest (geographically) landing sequence in the mission will be used. The Latitude/Longitude is optional, and may be set to 0/0 if not needed. If specified then it will be used to help find the closest landing sequence. |Empty| Empty| Empty| Empty| Latitude| Longitude| Empty|  */
	MAV_CMD_DO_RALLY_LAND = 190, /* Mission command to perform a landing from a rally point. |Break altitude (meters)| Landing speed (m/s)| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_GO_AROUND = 191, /* Mission command to safely abort an autonmous landing. |Altitude (meters)| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_REPOSITION = 192, /* Reposition the vehicle to a specific WGS84 global position. |Ground speed, less than 0 (-1) for default| Reserved| Reserved| Yaw heading, NaN for unchanged| Latitude (deg * 1E7)| Longitude (deg * 1E7)| Altitude (meters)|  */
	MAV_CMD_DO_PAUSE_CONTINUE = 193, /* If in a GPS controlled position mode, hold the current position or continue. |0: Pause current mission or reposition command, hold current position. 1: Continue mission. A VTOL capable vehicle should enter hover mode (multicopter and VTOL planes). A plane should loiter with the default loiter radius.| Reserved| Reserved| Reserved| Reserved| Reserved| Reserved|  */
	MAV_CMD_DO_CONTROL_VIDEO = 200, /* Control onboard camera system. |Camera ID (-1 for all)| Transmission: 0: disabled, 1: enabled compressed, 2: enabled raw| Transmission mode: 0: video stream, >0: single images every n seconds (decimal)| Recording: 0: disabled, 1: enabled compressed, 2: enabled raw| Empty| Empty| Empty|  */
	MAV_CMD_DO_SET_ROI = 201, /* Sets the region of interest (ROI) for a sensor set or the vehicle itself. This can then be used by the vehicles control system to control the vehicle attitude and the attitude of various sensors such as cameras. |Region of intereset mode. (see MAV_ROI enum)| MISSION index/ target ID. (see MAV_ROI enum)| ROI index (allows a vehicle to manage multiple ROI's)| Empty| x the location of the fixed ROI (see MAV_FRAME)| y| z|  */
	MAV_CMD_DO_DIGICAM_CONFIGURE = 202, /* Mission command to configure an on-board camera controller system. |Modes: P, TV, AV, M, Etc| Shutter speed: Divisor number for one second| Aperture: F stop number| ISO number e.g. 80, 100, 200, Etc| Exposure type enumerator| Command Identity| Main engine cut-off time before camera trigger in seconds/10 (0 means no cut-off)|  */
	MAV_CMD_DO_DIGICAM_CONTROL = 203, /* Mission command to control an on-board camera controller system. |Session control e.g. show/hide lens| Zoom's absolute position| Zooming step value to offset zoom from the current position| Focus Locking, Unlocking or Re-locking| Shooting Command| Command Identity| Empty|  */
	MAV_CMD_DO_MOUNT_CONFIGURE = 204, /* Mission command to configure a camera or antenna mount |Mount operation mode (see MAV_MOUNT_MODE enum)| stabilize roll? (1 = yes, 0 = no)| stabilize pitch? (1 = yes, 0 = no)| stabilize yaw? (1 = yes, 0 = no)| Empty| Empty| Empty|  */
	MAV_CMD_DO_MOUNT_CONTROL = 205, /* Mission command to control a camera or antenna mount |pitch or lat in degrees, depending on mount mode.| roll or lon in degrees depending on mount mode| yaw or alt (in meters) depending on mount mode| reserved| reserved| reserved| MAV_MOUNT_MODE enum value|  */
	MAV_CMD_DO_SET_CAM_TRIGG_DIST = 206, /* Mission command to set CAM_TRIGG_DIST for this flight |Camera trigger distance (meters)| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_FENCE_ENABLE = 207, /* Mission command to enable the geofence |enable? (0=disable, 1=enable, 2=disable_floor_only)| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_PARACHUTE = 208, /* Mission command to trigger a parachute |action (0=disable, 1=enable, 2=release, for some systems see PARACHUTE_ACTION enum, not in general message set.)| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_MOTOR_TEST = 209, /* Mission command to perform motor test |motor sequence number (a number from 1 to max number of motors on the vehicle)| throttle type (0=throttle percentage, 1=PWM, 2=pilot throttle channel pass-through. See MOTOR_TEST_THROTTLE_TYPE enum)| throttle| timeout (in seconds)| Empty| Empty| Empty|  */
	MAV_CMD_DO_INVERTED_FLIGHT = 210, /* Change to/from inverted flight |inverted (0=normal, 1=inverted)| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_GRIPPER = 211, /* Mission command to operate EPM gripper |gripper number (a number from 1 to max number of grippers on the vehicle)| gripper action (0=release, 1=grab. See GRIPPER_ACTIONS enum)| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_AUTOTUNE_ENABLE = 212, /* Enable/disable autotune |enable (1: enable, 0:disable)| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_MOUNT_CONTROL_QUAT = 220, /* Mission command to control a camera or antenna mount, using a quaternion as reference. |q1 - quaternion param #1, w (1 in null-rotation)| q2 - quaternion param #2, x (0 in null-rotation)| q3 - quaternion param #3, y (0 in null-rotation)| q4 - quaternion param #4, z (0 in null-rotation)| Empty| Empty| Empty|  */
	MAV_CMD_DO_GUIDED_MASTER = 221, /* set id of master controller |System ID| Component ID| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_GUIDED_LIMITS = 222, /* set limits for external control |timeout - maximum time (in seconds) that external controller will be allowed to control vehicle. 0 means no timeout| absolute altitude min (in meters, AMSL) - if vehicle moves below this alt, the command will be aborted and the mission will continue.  0 means no lower altitude limit| absolute altitude max (in meters)- if vehicle moves above this alt, the command will be aborted and the mission will continue.  0 means no upper altitude limit| horizontal move limit (in meters, AMSL) - if vehicle moves more than this distance from it's location at the moment the command was executed, the command will be aborted and the mission will continue. 0 means no horizontal altitude limit| Empty| Empty| Empty|  */
	MAV_CMD_DO_LAST = 240, /* NOP - This command is only used to mark the upper limit of the DO commands in the enumeration |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_PREFLIGHT_CALIBRATION = 241, /* Trigger calibration. This command will be only accepted if in pre-flight mode. |Gyro calibration: 0: no, 1: yes| Magnetometer calibration: 0: no, 1: yes| Ground pressure: 0: no, 1: yes| Radio calibration: 0: no, 1: yes| Accelerometer calibration: 0: no, 1: yes| Compass/Motor interference calibration: 0: no, 1: yes| Empty|  */
	MAV_CMD_PREFLIGHT_SET_SENSOR_OFFSETS = 242, /* Set sensor offsets. This command will be only accepted if in pre-flight mode. |Sensor to adjust the offsets for: 0: gyros, 1: accelerometer, 2: magnetometer, 3: barometer, 4: optical flow, 5: second magnetometer| X axis offset (or generic dimension 1), in the sensor's raw units| Y axis offset (or generic dimension 2), in the sensor's raw units| Z axis offset (or generic dimension 3), in the sensor's raw units| Generic dimension 4, in the sensor's raw units| Generic dimension 5, in the sensor's raw units| Generic dimension 6, in the sensor's raw units|  */
	MAV_CMD_PREFLIGHT_UAVCAN = 243, /* Trigger UAVCAN config. This command will be only accepted if in pre-flight mode. |1: Trigger actuator ID assignment and direction mapping.| Reserved| Reserved| Reserved| Reserved| Reserved| Reserved|  */
	MAV_CMD_PREFLIGHT_STORAGE = 245, /* Request storage of different parameter values and logs. This command will be only accepted if in pre-flight mode. |Parameter storage: 0: READ FROM FLASH/EEPROM, 1: WRITE CURRENT TO FLASH/EEPROM, 2: Reset to defaults| Mission storage: 0: READ FROM FLASH/EEPROM, 1: WRITE CURRENT TO FLASH/EEPROM, 2: Reset to defaults| Onboard logging: 0: Ignore, 1: Start default rate logging, -1: Stop logging, > 1: start logging with rate of param 3 in Hz (e.g. set to 1000 for 1000 Hz logging)| Reserved| Empty| Empty| Empty|  */
	MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN = 246, /* Request the reboot or shutdown of system components. |0: Do nothing for autopilot, 1: Reboot autopilot, 2: Shutdown autopilot, 3: Reboot autopilot and keep it in the bootloader until upgraded.| 0: Do nothing for onboard computer, 1: Reboot onboard computer, 2: Shutdown onboard computer, 3: Reboot onboard computer and keep it in the bootloader until upgraded.| Reserved, send 0| Reserved, send 0| Reserved, send 0| Reserved, send 0| Reserved, send 0|  */
	MAV_CMD_OVERRIDE_GOTO = 252, /* Hold / continue the current action |MAV_GOTO_DO_HOLD: hold MAV_GOTO_DO_CONTINUE: continue with next item in mission plan| MAV_GOTO_HOLD_AT_CURRENT_POSITION: Hold at current position MAV_GOTO_HOLD_AT_SPECIFIED_POSITION: hold at specified position| MAV_FRAME coordinate frame of hold point| Desired yaw angle in degrees| Latitude / X position| Longitude / Y position| Altitude / Z position|  */
	MAV_CMD_MISSION_START = 300, /* start running a mission |first_item: the first mission item to run| last_item:  the last mission item to run (after this item is run, the mission ends)|  */
	MAV_CMD_COMPONENT_ARM_DISARM = 400, /* Arms / Disarms a component |1 to arm, 0 to disarm|  */
	MAV_CMD_GET_HOME_POSITION = 410, /* Request the home position from the vehicle. |Reserved| Reserved| Reserved| Reserved| Reserved| Reserved| Reserved|  */
	MAV_CMD_START_RX_PAIR = 500, /* Starts receiver pairing |0:Spektrum| 0:Spektrum DSM2, 1:Spektrum DSMX|  */
	MAV_CMD_GET_MESSAGE_INTERVAL = 510, /* Request the interval between messages for a particular MAVLink message ID |The MAVLink message ID|  */
	MAV_CMD_SET_MESSAGE_INTERVAL = 511, /* Request the interval between messages for a particular MAVLink message ID. This interface replaces REQUEST_DATA_STREAM |The MAVLink message ID| The interval between two messages, in microseconds. Set to -1 to disable and 0 to request default rate.|  */
	MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES = 520, /* Request autopilot capabilities |1: Request autopilot version| Reserved (all remaining params)|  */
	MAV_CMD_IMAGE_START_CAPTURE = 2000, /* Start image capture sequence |Duration between two consecutive pictures (in seconds)| Number of images to capture total - 0 for unlimited capture| Resolution in megapixels (0.3 for 640x480, 1.3 for 1280x720, etc)|  */
	MAV_CMD_IMAGE_STOP_CAPTURE = 2001, /* Stop image capture sequence |Reserved| Reserved|  */
	MAV_CMD_DO_TRIGGER_CONTROL = 2003, /* Enable or disable on-board camera triggering system. |Trigger enable/disable (0 for disable, 1 for start)| Shutter integration time (in ms)| Reserved|  */
	MAV_CMD_VIDEO_START_CAPTURE = 2500, /* Starts video capture |Camera ID (0 for all cameras), 1 for first, 2 for second, etc.| Frames per second| Resolution in megapixels (0.3 for 640x480, 1.3 for 1280x720, etc)|  */
	MAV_CMD_VIDEO_STOP_CAPTURE = 2501, /* Stop the current video capture |Reserved| Reserved|  */
	MAV_CMD_PANORAMA_CREATE = 2800, /* Create a panorama at the current position |Viewing angle horizontal of the panorama (in degrees, +- 0.5 the total angle)| Viewing angle vertical of panorama (in degrees)| Speed of the horizontal rotation (in degrees per second)| Speed of the vertical rotation (in degrees per second)|  */
	MAV_CMD_DO_VTOL_TRANSITION = 3000, /* Request VTOL transition |The target VTOL state, as defined by ENUM MAV_VTOL_STATE. Only MAV_VTOL_STATE_MC and MAV_VTOL_STATE_FW can be used.|  */
	MAV_CMD_PAYLOAD_PREPARE_DEPLOY = 30001, /* Deploy payload on a Lat / Lon / Alt position. This includes the navigation to reach the required release position and velocity. |Operation mode. 0: prepare single payload deploy (overwriting previous requests), but do not execute it. 1: execute payload deploy immediately (rejecting further deploy commands during execution, but allowing abort). 2: add payload deploy to existing deployment list.| Desired approach vector in degrees compass heading (0..360). A negative value indicates the system can define the approach vector at will.| Desired ground speed at release time. This can be overriden by the airframe in case it needs to meet minimum airspeed. A negative value indicates the system can define the ground speed at will.| Minimum altitude clearance to the release position in meters. A negative value indicates the system can define the clearance at will.| Latitude unscaled for MISSION_ITEM or in 1e7 degrees for MISSION_ITEM_INT| Longitude unscaled for MISSION_ITEM or in 1e7 degrees for MISSION_ITEM_INT| Altitude, in meters AMSL|  */
	MAV_CMD_PAYLOAD_CONTROL_DEPLOY = 30002, /* Control the payload deployment. |Operation mode. 0: Abort deployment, continue normal mission. 1: switch to payload deploment mode. 100: delete first payload deployment request. 101: delete all payload deployment requests.| Reserved| Reserved| Reserved| Reserved| Reserved| Reserved|  */
	MAV_CMD_POWER_OFF_INITIATED = 42000, /* A system wide power-off event has been initiated. |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_SOLO_BTN_FLY_CLICK = 42001, /* FLY button has been clicked. |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_SOLO_BTN_FLY_HOLD = 42002, /* FLY button has been held for 1.5 seconds. |Takeoff altitude| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_SOLO_BTN_PAUSE_CLICK = 42003, /* PAUSE button has been clicked. |1 if Solo is in a shot mode, 0 otherwise| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_START_MAG_CAL = 42424, /* Initiate a magnetometer calibration |uint8_t bitmask of magnetometers (0 means all)| Automatically retry on failure (0=no retry, 1=retry).| Save without user input (0=require input, 1=autosave).| Delay (seconds)| Autoreboot (0=user reboot, 1=autoreboot)| Empty| Empty|  */
	MAV_CMD_DO_ACCEPT_MAG_CAL = 42425, /* Initiate a magnetometer calibration |uint8_t bitmask of magnetometers (0 means all)| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_CANCEL_MAG_CAL = 42426, /* Cancel a running magnetometer calibration |uint8_t bitmask of magnetometers (0 means all)| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_SET_FACTORY_TEST_MODE = 42427, /* Command autopilot to get into factory test/diagnostic mode |0 means get out of test mode, 1 means get into test mode| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_DO_SEND_BANNER = 42428, /* Reply with the version banner |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_GIMBAL_RESET = 42501, /* Causes the gimbal to reset and boot as if it was just powered on |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_GIMBAL_AXIS_CALIBRATION_STATUS = 42502, /* Reports progress and success or failure of gimbal axis calibration procedure |Gimbal axis we're reporting calibration progress for| Current calibration progress for this axis, 0x64=100%| Status of the calibration| Empty| Empty| Empty| Empty|  */
	MAV_CMD_GIMBAL_REQUEST_AXIS_CALIBRATION = 42503, /* Starts commutation calibration on the gimbal |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
	MAV_CMD_GIMBAL_FULL_RESET = 42505, /* Erases gimbal application and parameters |Magic number| Magic number| Magic number| Magic number| Magic number| Magic number| Magic number|  */
	MAV_CMD_ENUM_END = 42506, /*  | */
} MAV_CMD;

enum autopilot_modes {
	STABILIZE = 0,  // manual airframe angle with manual throttle
	ACRO = 1,  // manual body-frame angular rate with manual throttle
	ALT_HOLD = 2,  // manual airframe angle with automatic throttle
	AUTO = 3,  // fully automatic waypoint control using mission commands
	GUIDED = 4,  // fully automatic fly to coordinate or fly at velocity/direction using GCS immediate commands
	LOITER = 5,  // automatic horizontal acceleration with automatic throttle
	RTL = 6,  // automatic return to launching point
	CIRCLE = 7,  // automatic circular flight with automatic throttle
	LAND = 9,  // automatic landing with horizontal position control
	DRIFT = 11,  // semi-automous position, yaw and throttle control
	SPORT = 13,  // manual earth-frame angular rate control with manual throttle
	FLIP = 14,  // automatically flip the vehicle on the roll axis
	AUTOTUNE = 15,  // automatically tune the vehicle's roll and pitch gains
	POSHOLD = 16,  // automatic position hold with manual override, with automatic throttle
	BRAKE = 17,  // full-brake using inertial/GPS system, no pilot input
	THROW = 18   // throw to launch mode using inertial/GPS system, no pilot input
};


#define SYS_FLAG_INDEX_ARMING     (1 << 0)
#define SYS_FLAG_INDEX_LAND       (1 << 1)

#define AC_FENCE_TYPE_NONE                          0       // fence disabled
#define AC_FENCE_TYPE_ALT_MAX                       1       // high alt fence which usually initiates an RTL
#define AC_FENCE_TYPE_CIRCLE                        2       // circular horizontal fence (usually initiates an RTL)

#define AC_FENCE_ACTION_REPORT_ONLY                 0       // report to GCS that boundary has been breached but take no further action
#define AC_FENCE_ACTION_RTL_AND_LAND                1       // return to launch and, if that fails, land

#define TAST_STOP                       0
#define TAST_START                      1
#define TAST_RESUME                     2
#define TAST_SET_CUR                    3


#define VODEO_RES_3840x2160_30P_16_9    0xe0
#define VODEO_RES_2880x2160_30P_4_3     0xe1
#define VODEO_RES_2704x2028_30P_4_3     0xe2
#define VODEO_RES_2704x1520_60P_16_9    0xe3
#define VODEO_RES_2704x1520_30P_16_9    0xe4
#define VODEO_RES_2560x1440_60P_16_9    0xe5
#define VODEO_RES_2560x1440_30P_16_9    0xe6
#define VODEO_RES_1920x1440_60P_4_3     0xe7
#define VODEO_RES_1920x1440_30P_4_3     0xe8
#define VODEO_RES_1920x1080_120P_16_9   0xe9
#define VODEO_RES_1920x1080_60P_16_9    0xea
#define VODEO_RES_1920x1080_30P_16_9    0xeb
#define VODEO_RES_1280x960_120_P_4_3    0xec
#define VODEO_RES_1280x960_60_P_4_3     0xed
#define VODEO_RES_1280x960_30_P_4_3     0xee
#define VODEO_RES_1280x720_240P_16_9    0xef
#define VODEO_RES_1280x720_120P_16_9    0xf0
#define VODEO_RES_1280x720_60P_16_9     0xf1
#define VODEO_RES_1280x720_30P_16_9     0xf2

#define VODEO_QUALITY_SFINE     0xb1
#define VODEO_QUALITY_FINE      0xb2
#define VODEO_QUALITY_NORMAL    0xb3

#define DUAL_FILES_ON           0x01
#define DUAL_FILES_OFF          0x02

#define TIME_LAPSE_OFF          0xa1
#define TIME_LAPSE_1S           0xa2
#define TIME_LAPSE_2S           0xa3
#define TIME_LAPSE_5S           0xa4
#define TIME_LAPSE_10S          0xa5
#define TIME_LAPSE_30S          0xa6
#define TIME_LAPSE_60S          0xa7

#define LOOP_RECORD_OFF         0x60
#define LOOP_RECORD_2MIN        0x61
#define LOOP_RECORD_3MIN        0x62
#define LOOP_RECORD_5MIN        0x63

#define VIDEO_STAMP_OFF         0x51
#define VIDEO_STAMP_DATE        0x52
#define VIDEO_STAMP_TIME        0x53
#define VIDEO_STAMP_DTTM        0x54

#define EIS_ON                  0x01
#define EIS_OFF                 0x02

#define PHOTO_SIZE_16M          0xb1
#define PHOTO_SIZE_14M          0xb2
#define PHOTO_SIZE_12M          0xb3
#define PHOTO_SIZE_8_3M         0xb4
#define PHOTO_SIZE_5M           0xb5
#define PHOTO_SIZE_3M           0xb6

#define SELF_TIMER_OFF          0x31
#define SELF_TIMER_2S           0x32
#define SELF_TIMER_3S           0x33
#define SELF_TIMER_5S           0x34
#define SELF_TIMER_10S          0x35
#define SELF_TIMER_30S          0x36
#define SELF_TIMER_60S          0x37

#define BURST_RATE_OFF          0x41
#define BURST_RATE_3P           0x42
#define BURST_RATE_5P           0x43
#define BURST_RATE_10P          0x44

#define PHOTO_STAMP_OFF         0x01
#define PHOTO_STAMP_DATE        0x02
#define PHOTO_STAMP_TIME        0x03
#define PHOTO_STAMP_DTTM        0x04


#define LONG_EXPOSURE_OFF       0x01
#define LONG_EXPOSURE_1_30S     0x02
#define LONG_EXPOSURE_1S        0x03
#define LONG_EXPOSURE_2S        0x04
#define LONG_EXPOSURE_5S        0x05
#define LONG_EXPOSURE_10S       0x06
#define LONG_EXPOSURE_20S       0x07
#define LONG_EXPOSURE_30S       0x08
#define LONG_EXPOSURE_60S       0x09

#define SHARPNESS_STRONG        0x01
#define SHARPNESS_NORAML        0x02
#define SHARPNESS_SOFT          0x03

#define WHITE_BALANCE_AUTO         0x21
#define WHITE_BALANCE_INCANDESCENT 0x22
#define WHITE_BALANCE_D4000        0x23
#define WHITE_BALANCE_D5000        0x24
#define WHITE_BALANCE_DAYLIGHT     0x25
#define WHITE_BALANCE_CLOUDY       0x26
#define WHITE_BALANCE_D9000        0x27
#define WHITE_BALANCE_D10000       0x28
#define WHITE_BALANCE_FLASH        0x29
#define WHITE_BALANCE_FLUORESCENT  0x2a
#define WHITE_BALANCE_WATER        0x2b
#define WHITE_BALANCE_OUTDOOR      0x2c

#define EV_N2_0                 0x11
#define EV_N1_7                 0x12
#define EV_N1_3                 0x13
#define EV_N1_0                 0x14
#define EV_N0_7                 0x15
#define EV_N0_3                 0x16
#define EV_P0_0                 0x17
#define EV_P0_3                 0x18
#define EV_P0_7                 0x19
#define EV_P1_0                 0x1a
#define EV_P1_3                 0x1b
#define EV_P1_7                 0x1c
#define EV_P2_0                 0x1d

#define ISO_AUTO                0x01
#define ISO_100                 0x02
#define ISO_200                 0x03
#define ISO_400                 0x04
#define ISO_800                 0x05
#define ISO_1600                0x06

#define SCENE_MODE_NORMAL       0x01
#define SCENE_MODE_FLASH        0x02
#define SCENE_MODE_NIGHT        0x03
#define SCENE_MODE_SPORT        0x04
#define SCENE_MODE_LANDSCAPE    0x05
#define SCENE_MODE_PORTRAIT     0x06
#define SCENE_MODE_SUNSET       0x07

#define EFFECT_NORMAL           0x01
#define EFFECT_ART              0x02
#define EFFECT_SEPIA            0x03
#define EFFECT_NEGATIVE         0x04
#define EFFECT_B_W              0x05
#define EFFECT_VIVID            0x06
#define EFFECT_70FILM           0x07

#define METER_SPOT              0x01
#define METER_MULTI             0x02
#define METER_CENTER            0x03

#define STAMP_DISP_YMD          0x11
#define STAMP_DISP_DMY          0x12
#define STAMP_DISP_MDY          0x13

#define TV_MODE_NTSC            0x01
#define TV_MODE_PAL             0x02

#define LIGHT_FREQ_AUTO         0x01
#define LIGHT_FREQ_50HZ         0x02
#define LIGHT_FREQ_60HZ         0x03

#define TIMELAPSE_OFF           0x01
#define TIMELAPSE_500MS         0x02
#define TIMELAPSE_1S            0x03
#define TIMELAPSE_5S            0x04


typedef  unsigned char  uint8_t;
typedef  signed char    int8_t;
typedef  unsigned short uint16_t;
typedef  signed short   int16_t;
typedef  unsigned int   uint32_t;
typedef  signed int     int32_t;

#pragma pack (1)
struct ibus_message_t {
        uint8_t packetHead;
        uint16_t srcAddress : 2;
        uint16_t destAddress : 2;
        uint16_t length : 5;
        uint16_t type : 7;
        uint8_t  data[26];
} ;

struct ibus_rcdata_struct_t {
  uint8_t  d_ch1:1;
  uint8_t  d_ch2:1;
  uint8_t  d_ch3:1;
  uint8_t  d_ch4:1;
  uint8_t  d_ch5:1;
  uint8_t  d_ch6:1;
  uint8_t  d_ch7:1;
  uint8_t  d_ch8:1;

  uint16_t  a_ch1:12;
  uint16_t  a_ch2:12;
  uint16_t  a_ch3:12;
  uint16_t  a_ch4:12;
  uint16_t  a_ch5:12;
  uint16_t  a_ch6:12;
  uint16_t  a_ch7:12;

  int16_t  mag_x:12;
  int16_t  mag_y:12;
  int16_t  mag_z:12;

  int32_t longtitude;
  int32_t latitude;
} ;

struct ibus_sysstate_struct_t {
  uint8_t  flymode:2;
  uint8_t  subflymode:6;
  uint8_t  stars;
  int32_t  homelat;             //1E7
  int32_t  homelon;             //1E7
  int32_t  homealt;             //1E2
  uint32_t  datatime ;
  uint16_t navIndex;
  uint16_t stateflag;
} ;

struct ibus_attitude_struct_t {
  float  roll;           //0.01度
  float  pitch;          //0.01度
  float  yaw;            //0.01度
  float  rollspeed;      //0.01度/s
  float  pitchspeed;     //0.01度/s
  float  yawspeed;       //0.01度/sxiang'm
};

struct ibus_position_struct_t {
  int32_t  lat;             //1E7
  int32_t  lon;             //1E7
  int32_t  alt;             //1E2
  int32_t  relative_alt;    //1E2
  int16_t  vx;
  int16_t  vy;
  int16_t  vz;
  uint16_t hdg;
};

struct ibus_battery_struct_t {
  uint16_t  remainingCapacity;             //mAh
  uint16_t  voltage;               //mV
  uint16_t  current;             //mA
  uint16_t  remainingTime;       //s
  uint8_t   remainingPercent;    //%
} ;

struct ibus_camera_struct_t {
	uint8_t   cameramode;
	uint8_t   abnormalState;
	uint16_t  camerapitch;
	uint16_t  remainingnum;
	uint16_t  remainingTime;       //s
	uint16_t  remainingCapacity;   //mbyte
	uint16_t  totalCapacity;
	uint8_t   needSetTime;
} ;

struct ibus_torcdata_struct_t {
  uint8_t   stars;
  uint8_t   remainingCapacity;
  uint8_t   flymode;
  uint8_t   cameramode;
  uint16_t  remainingnum;
  uint16_t  remainingTime;       //s
  int16_t   relative_alt;        //cm
  int16_t   velocity;            //cm/s;
  int16_t   yaw;                 //0.01度
  uint16_t  distorc;
  uint16_t  camerapitch;
  uint16_t  remainingflyTime;    //s
};
struct ibus_ack_struct_t {
	uint8_t  type;
	uint16_t  exType;
	int32_t   retCode;
};


struct ibus_rcstate_struct_t {
	uint8_t   rcRemainingCapacity;
	uint8_t   rcMode;
	uint16_t  buffSize;
	uint16_t  remainingBuffSize;
};

struct ibus_missionitem_struct_t {
	uint16_t  total;
	uint16_t  seq;
	uint16_t  command;
	int32_t   x;
	int32_t   y;
	int16_t   z;
	uint8_t   variable[8];
} ;

#pragma pack()

struct mission_item_struct_t {
	int  total;
	int  seq;
	int  command;
	double   x;
	double   y;
	float    z;
	float    param[4];
};

struct ibus_retry_struct_t {
	int16_t  timesCount;
	int16_t  retryRates;
	int16_t  retryTicks;
	int16_t  packetSize;
	int      needACK;
	int      retCode;
	ibus_message_t ibusPacket;
};


class IbusMessage
{
public:
  IbusMessage(void);
  void update(uint8_t buff[],int size);
  void set_retry_rates(int times,int timeOut);
  int  update_ticks(int timeMs);
  int  get_data_readysend(void*buff,int size);
  int  get_ibus_ret(int id);
  int  set_plane_time(uint32_t time);
  int  set_arm_disarm(int armflag);
  int  set_plane_home(double lat, double lng, float alt);
  int  set_plane_mode(int mode);
  int  set_plane_land(void);
  int  set_plane_rtl(void);
  int  set_plane_takeoff(float alt);
  int  set_plane_rtlaltitude(float alt);
  int  set_plane_fenceparam(uint8_t en, uint8_t enf, uint8_t ac, float altmax, float  rmax);
  int  set_plane_runtast(uint8_t cmd, uint8_t index);

  int  send_write_tastitem(mission_item_struct_t *item);
  int  send_read_tastitem(int pointIndex);
  int  get_ret_tastitem(int id,mission_item_struct_t *item);
  int  send_tastitem_takeoff(uint16_t allPoint,uint16_t pointIndex,float alt);
  int  send_tastitem_land(uint16_t allPoint, uint16_t pointIndex);
  int  send_tastitem_rtl(uint16_t allPoint, uint16_t pointIndex);
  int  send_tastitem_circle(uint16_t allPoint, uint16_t pointIndex, double lat, double lon, float alt,int16_t r, uint16_t times);
  int  send_tastitem_waypoint(uint16_t allPoint, uint16_t pointIndex,double lat,double lon,float alt, uint16_t delayS);
 
  int send_camera_photo();   // 拍照
  int send_camera_video();   // 录像
  int send_camera_pitch(float pitch);  // 调整角度
  int send_camera_video_res(int res);
  int send_camera_video_quality(int quality);
  int send_camera_dual_files(int dualfiles);
  int send_camera_time_lapse(int timelapse);
  int send_camera_loop_record(int looprecord);
  int send_camera_video_stamp(int videostamp);
  int send_camera_eis(int eis);
  int send_camera_photo_size(int photosize);
  int send_camera_self_timer(int selftimer);
  int send_camera_burst_rate(int burstrate);
  int send_camera_photo_stamp(int photostamp);
  int send_camera_long_exposure(int longexposure);
  int send_camera_sharpness(int sharpness);
  int send_camera_white_balance(int whitebalance);
  int send_camera_ev(int ev);
  int send_camera_iso(int iso);
  int send_camera_scene_mode(int scenemode);
  int send_camera_effect(int effect);
  int send_camera_meter(int meter);
  int send_camera_stamp_disp(int stampdisp);
  int send_camera_tv_mode(int tvmode);
  int send_camera_light_freq(int lightfreq);
  int send_camera_format();
  int send_camera_default();
  int send_camera_time_ymdhms(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
  int send_camera_time_ym(uint8_t year, uint8_t month);
  int send_camera_time_dh(uint8_t day, uint8_t hour);
  int send_camera_time_ms(uint8_t minute, uint8_t second);
  int send_camera_photo_timelapse(int timelapse);
  int send_camera_remote_code();

  int send_rc_remote_code(int code);
  int send_rc_mode(int mode);
  int send_rc_set_threshold(int threshold);

  float get_attitude_roll(void)      { return RadToEuler(this->attitudeData.roll); }
  float get_attitude_pitch(void)     { return RadToEuler(this->attitudeData.pitch);}
  float get_attitude_yaw(void)       { float temp = RadToEuler(this->attitudeData.yaw); return temp > 0 ? temp : temp + 360; }
  float get_attitude_rollspeed(void) { return RadToEuler(this->attitudeData.rollspeed); }
  float get_attitude_pitchspeed(void){ return RadToEuler(this->attitudeData.pitchspeed); }
  float get_attitude_yawspeed(void)  { return RadToEuler(this->attitudeData.yawspeed); }
  int   get_sysstate_flymode(void)   { return this->sysstateData.flymode; }
  int   get_sysstate_subflymode(void){ return this->sysstateData.subflymode; }
  int   get_sysstate_stars(void)     { return this->sysstateData.stars; }
  double get_sysstate_homelat(void)  { return this->sysstateData.homelat * 1E-7; }            //1E7
  double get_sysstate_homelon(void)  { return this->sysstateData.homelon * 1E-7; }                  //1E7
  float get_sysstate_homealt(void)   { return this->sysstateData.homealt * 0.01f; }             //1E2
  uint32_t get_sysstate_datetime(void) { return this->sysstateData.datatime; }
  uint32_t get_sysstate_stateflag(void) { return this->sysstateData.stateflag | (this->sysstateData.navIndex << 16); }
  double get_position_lat(void)     { return this->positionData.lat * 1E-7; }          //1E7
  double get_position_lon(void)     { return this->positionData.lon * 1E-7; }              //1E7
  float  get_position_alt(void)     { return this->positionData.alt * 0.01f; }             //1E2
  float  get_position_relative_alt(void) { return this->positionData.relative_alt * 0.01f; }    //1E2
  float  get_position_vx(void) { return this->positionData.vx * 0.01f; }
  float  get_position_vy(void) { return this->positionData.vy * 0.01f; }
  float  get_position_vz(void) { return this->positionData.vz * 0.01f; }
  float  get_position_hdg(void) { return this->positionData.hdg * 0.01f; }
  int    get_battery_remainingCapacity(void) { return this->batteryData.remainingCapacity; }              //mAh
  float  get_battery_voltage(void) { return this->batteryData.voltage * 0.001f; }                 //mV
  float  get_battery_current(void) { return this->batteryData.current * 0.001f; }               //mA
  int    get_battery_remainingTime(void) { return this->batteryData.remainingTime; }         //s
  int    get_battery_remainingPercent(void) { return this->batteryData.remainingPercent; }      //%
  int    get_camera_cameramode(void) { return this->cameraData.cameramode; }
  float  get_camera_camerapitch(void) { return this->cameraData.camerapitch * 0.01f; }
  int    get_camera_remainingnum(void) { return this->cameraData.remainingnum; }
  int    get_camera_remainingTime(void) { return this->cameraData.remainingTime; }         //s
  float  get_camera_remainingCapacity(void) { return this->cameraData.remainingCapacity; }
  int    get_camera_totalCapacityp(void) { return this->cameraData.totalCapacity; }
  int    get_camera_needSetTime(void) { return this->cameraData.needSetTime; }

  int    get_rc_mode(void) { return this->rcstateData.rcMode; };
protected:
  int    checkIbusPacket(ibus_message_t *p_IbusMessage,int length);
  uint16_t dataCheckSum(ibus_message_t *p_IbusMessage);
  void analysisIbusPacket(ibus_message_t *p_IbusMessage);
  void receiveRcMessage(ibus_message_t *p_IbusMessage);
  void receiveAttitudeMessage(ibus_message_t *p_IbusMessage);
  void receiveSysstateMessage(ibus_message_t *p_IbusMessage);
  void receivePositionMessage(ibus_message_t *p_IbusMessage);
  void receiveBatteryMessage(ibus_message_t *p_IbusMessage);
  void receiveCameraMessage(ibus_message_t *p_IbusMessage);
  void receiveAckMessage(ibus_message_t *p_IbusMessage);
  void receiveTastItemMessage(ibus_message_t *p_IbusMessage);
  void receiveStringMessage(ibus_message_t *p_IbusMessage);
  void receiveRcStateMessage(ibus_message_t *p_IbusMessage);
  /* void sendAttitudeInf(void);
  void sendSysstateInf(void);
  void sendPositionInf(void);
  void sendBatteryInf(void);
  void sendCameraInf(void);
  void sendTorcdataInf(void);
  */
  int findRetryBuffIndex(void);
  int sendIbusMessage(uint8_t type,void *buff,uint8_t size,uint8_t desAddr);
  int sendIbusMessageSize(uint8_t type, void *buff, uint8_t size, uint8_t desAddr);
private:
  int _dataInBuff;
  uint8_t _dataBuff[DATA_BUFF_SIZE];

  ibus_sysstate_struct_t sysstateData;
  ibus_attitude_struct_t attitudeData;
  ibus_position_struct_t positionData;
  ibus_battery_struct_t  batteryData;
  ibus_camera_struct_t   cameraData;
  ibus_torcdata_struct_t torcdataData;
  ibus_rcstate_struct_t rcstateData;
  ibus_retry_struct_t ibusRetryBuff[ALL_IBUS_MSG_COUNT];
  mission_item_struct_t missionItem;
  ibus_rcdata_struct_t  rcData;
  int16_t  retryRates;
  int16_t  retryTimes;
  uint32_t sequence;
//uint8_t streamTicks[ALL_IBUS_MSG_COUNT];
//  uint8_t streamRates[ALL_IBUS_MSG_COUNT];
};
