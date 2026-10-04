#include "CASIO_GUI/casio.hpp"

struct MotorState
{
    int speed = 0;
    bool enabled = false;
    casio::Point position{0, 0};
    casio::Color color{255, 255, 255};
};

static void saveMotor(
    casio::ObjectFile& save,
    const std::string& name,
    const MotorState& motor)
{
    save.beginObject(
        name,
        "MotorState",
        1
    );

    save.set("speed", motor.speed);
    save.set("enabled", motor.enabled);
    save.set("position", motor.position);
    save.set("color", motor.color);

    save.endObject();
}

static bool loadMotor(
    casio::ObjectFile& save,
    const std::string& name,
    MotorState& motor)
{
    if(!save.selectObject(name))
        return false;

    // Optional type/version checks.
    if(save.getObjectType(name) != "MotorState")
    {
        save.endObject();
        return false;
    }

    motor.speed =
        save.getIntOr(
            "speed",
            motor.speed
        );

    motor.enabled =
        save.getBoolOr(
            "enabled",
            motor.enabled
        );

    save.get(
        "position",
        motor.position
    );

    save.get(
        "color",
        motor.color
    );

    save.endObject();
    return true;
}

int main()
{
    MotorState motor1;
    motor1.speed = 1200;
    motor1.enabled = true;
    motor1.position = casio::Point{40, 80};
    motor1.color = casio::Color{255, 0, 0};

    MotorState motor2;
    motor2.speed = 850;
    motor2.enabled = false;
    motor2.position = casio::Point{120, 80};
    motor2.color = casio::Color{0, 0, 255};

    casio::ObjectFile save;

    saveMotor(
        save,
        "motor1",
        motor1
    );

    saveMotor(
        save,
        "motor2",
        motor2
    );

    save.save(
        "/motors.ini"
    );


    // Reload example.
    casio::ObjectFile loaded;

    if(loaded.load(
        "/motors.ini"))
    {
        MotorState restored;

        loadMotor(
            loaded,
            "motor1",
            restored
        );
    }

    return 1;
}
