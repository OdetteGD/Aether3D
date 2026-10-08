#include <jni.h>



#include <android/log.h>



#include <algorithm>



#include <array>



#include <atomic>



#include <cmath>



#include <cstdint>



#include <iomanip>



#include <limits>



#include <mutex>



#include <sstream>



#include <string>



#include <vector>



#include <utility>



namespace {



constexpr float kPi =
    3.14159265358979323846f;



constexpr float kEpsilon =
    1.0e-6f;



constexpr float kGravity =
    -9.80665f;



constexpr float kGroundHeight =
    -1.0f;



constexpr const char* kLogTag =
    "Aether3D.Physics";



struct Vec3 {

    float x = 0.0f;

    float y = 0.0f;

    float z = 0.0f;



    Vec3() = default;



    Vec3(
        float xValue,
        float yValue,
        float zValue
    )
        : x(xValue),
          y(yValue),
          z(zValue) {
    }



    Vec3 operator+(
        const Vec3& other
    ) const {

        return Vec3(
            x + other.x,
            y + other.y,
            z + other.z
        );

    }



    Vec3 operator-(
        const Vec3& other
    ) const {

        return Vec3(
            x - other.x,
            y - other.y,
            z - other.z
        );

    }



    Vec3 operator*(
        float scalar
    ) const {

        return Vec3(
            x * scalar,
            y * scalar,
            z * scalar
        );

    }



    Vec3 operator/(
        float scalar
    ) const {

        if (
            std::fabs(
                scalar
            ) <= kEpsilon
        ) {

            return Vec3();

        }



        return Vec3(
            x / scalar,
            y / scalar,
            z / scalar
        );

    }



    Vec3& operator+=(
        const Vec3& other
    ) {

        x += other.x;

        y += other.y;

        z += other.z;



        return *this;

    }



    float lengthSquared() const {

        return
            x * x
            +
            y * y
            +
            z * z;

    }



    float length() const {

        return std::sqrt(
            lengthSquared()
        );

    }



    Vec3 normalized() const {

        const float magnitude =
            length();



        if (
            magnitude <= kEpsilon
        ) {

            return Vec3(
                0.0f,
                1.0f,
                0.0f
            );

        }



        return
            *this
            /
            magnitude;

    }



    static float dot(
        const Vec3& a,
        const Vec3& b
    ) {

        return
            a.x * b.x
            +
            a.y * b.y
            +
            a.z * b.z;

    }



    static Vec3 cross(
        const Vec3& a,
        const Vec3& b
    ) {

        return Vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );

    }

};



struct Quaternion {

    float x = 0.0f;

    float y = 0.0f;

    float z = 0.0f;

    float w = 1.0f;



    Quaternion() = default;



    Quaternion(
        float xValue,
        float yValue,
        float zValue,
        float wValue
    )
        : x(xValue),
          y(yValue),
          z(zValue),
          w(wValue) {
    }



    Quaternion normalized() const {

        const float magnitude =
            std::sqrt(
                x * x
                +
                y * y
                +
                z * z
                +
                w * w
            );



        if (
            magnitude <= kEpsilon
        ) {

            return Quaternion();

        }



        return Quaternion(
            x / magnitude,
            y / magnitude,
            z / magnitude,
            w / magnitude
        );

    }



    Quaternion operator*(
        const Quaternion& other
    ) const {

        return Quaternion(
            w * other.x
                +
            x * other.w
                +
            y * other.z
                -
            z * other.y,

            w * other.y
                -
            x * other.z
                +
            y * other.w
                +
            z * other.x,

            w * other.z
                +
            x * other.y
                -
            y * other.x
                +
            z * other.w,

            w * other.w
                -
            x * other.x
                -
            y * other.y
                -
            z * other.z
        );

    }



    Vec3 rotate(
        const Vec3& vector
    ) const {

        Quaternion point(
            vector.x,
            vector.y,
            vector.z,
            0.0f
        );



        Quaternion inverse(
            -x,
            -y,
            -z,
            w
        );



        Quaternion rotated =
            (
                *this
                *
                point
                *
                inverse
            );



        return Vec3(
            rotated.x,
            rotated.y,
            rotated.z
        );

    }

};



struct Mat4 {

    std::array<float, 16> values {};



    static Mat4 identity() {

        Mat4 matrix;



        matrix.values[0] =
            1.0f;



        matrix.values[5] =
            1.0f;



        matrix.values[10] =
            1.0f;



        matrix.values[15] =
            1.0f;



        return matrix;

    }

};



Mat4 makeTransformMatrix(
    const Vec3& position,
    const Quaternion& orientation
) {

    const Quaternion rotation =
        orientation.normalized();



    const float xx =
        rotation.x * rotation.x;



    const float yy =
        rotation.y * rotation.y;



    const float zz =
        rotation.z * rotation.z;



    const float xy =
        rotation.x * rotation.y;



    const float xz =
        rotation.x * rotation.z;



    const float yz =
        rotation.y * rotation.z;



    const float wx =
        rotation.w * rotation.x;



    const float wy =
        rotation.w * rotation.y;



    const float wz =
        rotation.w * rotation.z;



    Mat4 matrix =
        Mat4::identity();



    matrix.values[0] =
        1.0f - 2.0f * (yy + zz);



    matrix.values[1] =
        2.0f * (xy + wz);



    matrix.values[2] =
        2.0f * (xz - wy);



    matrix.values[4] =
        2.0f * (xy - wz);



    matrix.values[5] =
        1.0f - 2.0f * (xx + zz);



    matrix.values[6] =
        2.0f * (yz + wx);



    matrix.values[8] =
        2.0f * (xz + wy);



    matrix.values[9] =
        2.0f * (yz - wx);



    matrix.values[10] =
        1.0f - 2.0f * (xx + yy);



    matrix.values[12] =
        position.x;



    matrix.values[13] =
        position.y;



    matrix.values[14] =
        position.z;



    return matrix;

}



struct RigidBody {

    Vec3 position;

    Vec3 linearVelocity;



    Vec3 angularVelocity;



    Vec3 forceAccumulator;



    Vec3 torqueAccumulator;



    Quaternion orientation;



    float inverseMass =
        1.0f;



    Vec3 inverseInertiaBody {
        1.0f,
        1.0f,
        1.0f
    };



    void applyForce(
        const Vec3& force
    ) {

        forceAccumulator +=
            force;

    }



    void applyTorque(
        const Vec3& torque
    ) {

        torqueAccumulator +=
            torque;

    }



    Vec3 worldInverseInertia(
        const Vec3& angularVelocityVector
    ) const {

        const Vec3 bodyAngular =
            orientation
                .normalized()
                .rotate(
                    angularVelocityVector
                );



        const Vec3 bodyResult(
            bodyAngular.x
                * inverseInertiaBody.x,

            bodyAngular.y
                * inverseInertiaBody.y,

            bodyAngular.z
                * inverseInertiaBody.z
        );



        Quaternion inverseOrientation(
            -orientation.x,
            -orientation.y,
            -orientation.z,
            orientation.w
        );



        return inverseOrientation
            .normalized()
            .rotate(
                bodyResult
            );

    }



    void integrate(
        float deltaSeconds
    ) {

        applyForce(
            Vec3(
                0.0f,
                kGravity / inverseMass,
                0.0f
            )
        );



        const Vec3 linearAcceleration =
            forceAccumulator
            *
            inverseMass;



        linearVelocity +=
            linearAcceleration
            *
            deltaSeconds;



        position +=
            linearVelocity
            *
            deltaSeconds;



        const Vec3 angularAcceleration =
            worldInverseInertia(
                torqueAccumulator
            );



        angularVelocity +=
            angularAcceleration
            *
            deltaSeconds;



        Quaternion angularQuaternion(
            angularVelocity.x,
            angularVelocity.y,
            angularVelocity.z,
            0.0f
        );



        const Quaternion orientationDerivative =
            angularQuaternion
            *
            orientation;



        orientation.x +=
            0.5f
            *
            orientationDerivative.x
            *
            deltaSeconds;



        orientation.y +=
            0.5f
            *
            orientationDerivative.y
            *
            deltaSeconds;



        orientation.z +=
            0.5f
            *
            orientationDerivative.z
            *
            deltaSeconds;



        orientation.w +=
            0.5f
            *
            orientationDerivative.w
            *
            deltaSeconds;



        orientation =
            orientation.normalized();



        if (
            position.y < kGroundHeight
        ) {

            position.y =
                kGroundHeight;



            if (
                linearVelocity.y < 0.0f
            ) {

                linearVelocity.y =
                    -linearVelocity.y
                    *
                    0.05f;

            }

        }



        forceAccumulator =
            Vec3();



        torqueAccumulator =
            Vec3();

    }

};



struct VehicleWheel {

    Vec3 localPosition;



    float wheelRadius =
        0.32f;



    float suspensionRestLength =
        0.32f;



    float suspensionTravel =
        0.18f;



    float suspensionStiffness =
        32000.0f;



    float suspensionDamping =
        4200.0f;



    float steeringAngle =
        0.0f;



    float wheelAngularSpeed =
        0.0f;



    float normalLoad =
        0.0f;



    float longitudinalForce =
        0.0f;



    float lateralForce =
        0.0f;

};



struct VehicleState {

    float wheelBase =
        2.7f;



    float trackWidth =
        1.55f;



    float mass =
        1450.0f;



    float centerOfMassHeight =
        0.55f;



    float engineRpm =
        1100.0f;



    float throttle =
        0.0f;



    float brake =
        0.0f;



    float steeringInput =
        0.0f;



    std::array<float, 7> torqueCurve {

        110.0f,

        165.0f,

        220.0f,

        255.0f,

        245.0f,

        210.0f,

        165.0f

    };



    std::array<float, 7> torqueCurveRpm {

        1000.0f,

        2000.0f,

        3000.0f,

        4000.0f,

        5000.0f,

        6000.0f,

        7000.0f

    };



    std::array<VehicleWheel, 4> wheels;



    Vec3 linearVelocity;



    float yawRate =
        0.0f;



    void initialize() {

        wheels[0].localPosition =
            Vec3(
                wheelBase * 0.5f,
                0.0f,
                trackWidth * 0.5f
            );



        wheels[1].localPosition =
            Vec3(
                wheelBase * 0.5f,
                0.0f,
                -trackWidth * 0.5f
            );



        wheels[2].localPosition =
            Vec3(
                -wheelBase * 0.5f,
                0.0f,
                trackWidth * 0.5f
            );



        wheels[3].localPosition =
            Vec3(
                -wheelBase * 0.5f,
                0.0f,
                -trackWidth * 0.5f
            );

    }



    float engineTorque(
        float rpm
    ) const {

        if (
            rpm <= torqueCurveRpm.front()
        ) {

            return torqueCurve.front();

        }



        if (
            rpm >= torqueCurveRpm.back()
        ) {

            return torqueCurve.back();

        }



        for (
            std::size_t index = 0;
            index + 1 < torqueCurveRpm.size();
            ++index
        ) {

            if (
                rpm >= torqueCurveRpm[index]
                &&
                rpm <= torqueCurveRpm[index + 1]
            ) {

                const float range =
                    torqueCurveRpm[index + 1]
                    -
                    torqueCurveRpm[index];



                const float t =
                    (
                        rpm
                        -
                        torqueCurveRpm[index]
                    )
                    /
                    range;



                return
                    torqueCurve[index]
                    +
                    (
                        torqueCurve[index + 1]
                        -
                        torqueCurve[index]
                    )
                    *
                    t;

            }

        }



        return torqueCurve.front();

    }



    void calculateAckermann() {

        const float maxSteeringAngle =
            0.62f;



        const float centerAngle =
            steeringInput
            *
            maxSteeringAngle;



        if (
            std::fabs(
                centerAngle
            ) <= kEpsilon
        ) {

            wheels[0].steeringAngle =
                0.0f;

            wheels[1].steeringAngle =
                0.0f;

            return;

        }



        const float turningRadius =
            wheelBase
            /
            std::tan(
                centerAngle
            );



        const float innerRadius =
            std::max(
                0.1f,
                std::fabs(
                    turningRadius
                )
                -
                trackWidth * 0.5f
            );



        const float outerRadius =
            std::max(
                0.1f,
                std::fabs(
                    turningRadius
                )
                +
                trackWidth * 0.5f
            );



        const float innerAngle =
            std::atan(
                wheelBase
                /
                innerRadius
            );



        const float outerAngle =
            std::atan(
                wheelBase
                /
                outerRadius
            );



        if (
            centerAngle > 0.0f
        ) {

            wheels[0].steeringAngle =
                innerAngle;



            wheels[1].steeringAngle =
                outerAngle;

        } else {

            wheels[0].steeringAngle =
                -outerAngle;



            wheels[1].steeringAngle =
                -innerAngle;

        }

    }



    static float tireForce(
        float slip
    ) {

        const float peakForce =
            1.15f;



        const float stiffness =
            8.0f;



        return
            peakForce
            *
            std::tanh(
                stiffness
                *
                slip
            );

    }



    void simulate(
        float deltaSeconds
    ) {

        calculateAckermann();



        const float drivenTorque =
            engineTorque(
                engineRpm
            )
            *
            throttle;



        const float finalDrive =
            3.9f;



        const float gearRatio =
            3.2f;



        const float drivetrainTorque =
            drivenTorque
            *
            finalDrive
            *
            gearRatio
            *
            0.88f;



        const float wheelBaseLoadSplit =
            0.5f
            -
            centerOfMassHeight
            *
            linearVelocity.x
            *
            0.0015f;



        const float clampedSplit =
            std::clamp(
                wheelBaseLoadSplit,
                0.35f,
                0.65f
            );



        for (
            std::size_t wheelIndex = 0;
            wheelIndex < wheels.size();
            ++wheelIndex
        ) {

            VehicleWheel& wheel =
                wheels[wheelIndex];



            const float restToGround =
                wheel.localPosition.y
                -
                wheel.wheelRadius
                -
                kGroundHeight;



            const float compression =
                std::clamp(
                    wheel.suspensionRestLength
                    -
                    restToGround,
                    0.0f,
                    wheel.suspensionTravel
                );



            const float compressionVelocity =
                -compression
                /
                std::max(
                    deltaSeconds,
                    1.0e-4f
                );



            const float suspensionForce =
                compression
                *
                wheel.suspensionStiffness
                +
                compressionVelocity
                *
                wheel.suspensionDamping;



            wheel.normalLoad =
                std::max(
                    0.0f,
                    suspensionForce
                )
                +
                mass
                *
                -kGravity
                *
                (
                    wheelIndex < 2
                        ? clampedSplit * 0.5f
                        : (1.0f - clampedSplit) * 0.5f
                );



            const float forwardSpeed =
                linearVelocity.x;



            const float lateralSpeed =
                linearVelocity.z
                +
                yawRate
                *
                wheel.localPosition.x;



            const float wheelCircumference =
                2.0f
                *
                kPi
                *
                wheel.wheelRadius;



            const float wheelLinearSpeed =
                wheel.wheelAngularSpeed
                *
                wheel.wheelRadius;



            const float slipRatio =
                (
                    wheelLinearSpeed
                    -
                    forwardSpeed
                )
                /
                std::max(
                    1.0f,
                    std::fabs(
                        forwardSpeed
                    )
                );



            const float slipAngle =
                std::atan2(
                    lateralSpeed,
                    std::max(
                        0.1f,
                        std::fabs(
                            forwardSpeed
                        )
                    )
                );



            const float longitudinalGrip =
                tireForce(
                    slipRatio
                );



            const float lateralGrip =
                tireForce(
                    slipAngle
                );



            wheel.longitudinalForce =
                longitudinalGrip
                *
                wheel.normalLoad;



            wheel.lateralForce =
                -lateralGrip
                *
                wheel.normalLoad;



            const bool drivenAxle =
                wheelIndex >= 2;



            if (
                drivenAxle
            ) {

                const float axleShare =
                    drivetrainTorque
                    *
                    0.5f;



                const float wheelTorque =
                    axleShare
                    -
                    brake
                    *
                    650.0f
                    *
                    (
                        wheel.wheelAngularSpeed >= 0.0f
                            ? 1.0f
                            : -1.0f
                    );



                const float wheelInertia =
                    1.8f;



                const float angularAcceleration =
                    wheelTorque
                    /
                    wheelInertia;



                wheel.wheelAngularSpeed +=
                    angularAcceleration
                    *
                    deltaSeconds;



                wheel.wheelAngularSpeed *=
                    std::exp(
                        -deltaSeconds
                        *
                        1.75f
                    );

            }



            if (
                wheelCircumference > kEpsilon
            ) {

                engineRpm =
                    std::clamp(
                        std::fabs(
                            wheel.wheelAngularSpeed
                        )
                        *
                        wheelCircumference
                        *
                        30.0f
                        /
                        kPi,

                        900.0f,

                        7000.0f
                    );

            }

        }

    }

};



struct Aabb {

    Vec3 minimum;

    Vec3 maximum;

};



struct FracturePiece {

    Aabb bounds;

    Vec3 impactNormal;



    float impactEnergy =
        0.0f;

};



std::vector<FracturePiece>
fractureAabbByImpactPlane(
    const Aabb& bounds,
    const Vec3& impactPoint,
    const Vec3& impactNormal,
    float impactEnergy
) {

    std::vector<FracturePiece> pieces;



    const Vec3 extents =
        bounds.maximum
        -
        bounds.minimum;



    const Vec3 normal =
        impactNormal.normalized();



    std::array<float, 3> absNormal {

        std::fabs(
            normal.x
        ),

        std::fabs(
            normal.y
        ),

        std::fabs(
            normal.z
        )

    };



    std::size_t splitAxis =
        0;



    if (
        absNormal[1] > absNormal[splitAxis]
    ) {

        splitAxis =
            1;

    }



    if (
        absNormal[2] > absNormal[splitAxis]
    ) {

        splitAxis =
            2;

    }



    const float minimumCoordinate =
        splitAxis == 0
            ? bounds.minimum.x
            : splitAxis == 1
                ? bounds.minimum.y
                : bounds.minimum.z;



    const float maximumCoordinate =
        splitAxis == 0
            ? bounds.maximum.x
            : splitAxis == 1
                ? bounds.maximum.y
                : bounds.maximum.z;



    const float impactCoordinate =
        splitAxis == 0
            ? impactPoint.x
            : splitAxis == 1
                ? impactPoint.y
                : impactPoint.z;



    const float splitCoordinate =
        std::clamp(
            impactCoordinate,
            minimumCoordinate + 0.05f,
            maximumCoordinate - 0.05f
        );



    Aabb first =
        bounds;



    Aabb second =
        bounds;



    if (
        splitAxis == 0
    ) {

        first.maximum.x =
            splitCoordinate;



        second.minimum.x =
            splitCoordinate;

    } else if (
        splitAxis == 1
    ) {

        first.maximum.y =
            splitCoordinate;



        second.minimum.y =
            splitCoordinate;

    } else {

        first.maximum.z =
            splitCoordinate;



        second.minimum.z =
            splitCoordinate;

    }



    pieces.push_back(
        FracturePiece {
            first,
            normal,
            impactEnergy
                *
                (
                    extents.length()
                    /
                    std::max(
                        extents.length(),
                        0.001f
                    )
                )
                *
                0.5f
        }
    );



    pieces.push_back(
        FracturePiece {
            second,
            normal * -1.0f,
            impactEnergy * 0.5f
        }
    );



    return pieces;

}



struct IKSolution {

    Vec3 root;

    Vec3 jointOne;

    Vec3 jointTwo;

    Vec3 endEffector;

};



IKSolution solveThreeJointIk(
    const Vec3& root,
    const Vec3& target,
    float lengthOne,
    float lengthTwo,
    float lengthThree,
    const Vec3& poleDirection
) {

    const Vec3 targetDirection =
        (
            target
            -
            root
        )
        .normalized();



    const float totalReach =
        lengthOne
        +
        lengthTwo
        +
        lengthThree;



    const float targetDistance =
        std::clamp(
            (
                target
                -
                root
            ).length(),
            kEpsilon,
            totalReach - kEpsilon
        );



    const Vec3 wristTarget =
        root
        +
        targetDirection
        *
        std::clamp(
            targetDistance
            -
            lengthThree,
            kEpsilon,
            lengthOne
                +
            lengthTwo
                -
            kEpsilon
        );



    Vec3 pole =
        poleDirection.normalized();



    Vec3 planeNormal =
        Vec3::cross(
            targetDirection,
            pole
        );



    if (
        planeNormal.lengthSquared()
        <=
        kEpsilon
    ) {

        planeNormal =
            Vec3::cross(
                targetDirection,
                Vec3(
                    0.0f,
                    1.0f,
                    0.0f
                )
            );



        if (
            planeNormal.lengthSquared()
            <=
            kEpsilon
        ) {

            planeNormal =
                Vec3::cross(
                    targetDirection,
                    Vec3(
                        1.0f,
                        0.0f,
                        0.0f
                    )
                );

            }

        }



    planeNormal =
        planeNormal.normalized();



    Vec3 bendDirection =
        Vec3::cross(
            planeNormal,
            targetDirection
        )
        .normalized();



    const float wristDistance =
        (
            wristTarget
            -
            root
        ).length();



    const float cosineAtRoot =
        std::clamp(
            (
                lengthOne
                *
                lengthOne
                +
                wristDistance
                *
                wristDistance
                -
                lengthTwo
                *
                lengthTwo
            )
            /
            std::max(
                2.0f
                *
                lengthOne
                *
                wristDistance,
                kEpsilon
            ),
            -1.0f,
            1.0f
        );



    const float rootAngle =
        std::acos(
            cosineAtRoot
        );



    const Vec3 rootToWrist =
        (
            wristTarget
            -
            root
        )
        .normalized();



    const Vec3 jointOne =
        root
        +
        rootToWrist
        *
        (
            std::cos(
                rootAngle
            )
            *
            lengthOne
        )
        +
        bendDirection
        *
        (
            std::sin(
                rootAngle
            )
            *
            lengthOne
        );



    const Vec3 jointTwo =
        wristTarget;



    const Vec3 endEffector =
        jointTwo
        +
        targetDirection
        *
        lengthThree;



    return IKSolution {
        root,
        jointOne,
        jointTwo,
        endEffector
    };

}



struct CharacterController {

    Vec3 position;



    Vec3 velocity;



    float radius =
        0.35f;



    float halfHeight =
        0.9f;



    float moveAcceleration =
        28.0f;



    float moveDamping =
        10.0f;



    float maxSpeed =
        5.5f;



    float jumpSpeed =
        5.2f;



    bool grounded =
        false;



    void reset(
        const Vec3& startPosition
    ) {

        position =
            startPosition;



        velocity =
            Vec3();



        grounded =
            false;

    }



    void step(
        const Vec3& desiredVelocity,
        bool jumpRequested,
        float deltaSeconds
    ) {

        const Vec3 desiredHorizontal(
            desiredVelocity.x,
            0.0f,
            desiredVelocity.z
        );



        const float desiredSpeed =
            desiredHorizontal.length();



        Vec3 clampedDesired =
            desiredHorizontal;



        if (
            desiredSpeed > maxSpeed
        ) {

            clampedDesired =
                desiredHorizontal
                *
                (
                    maxSpeed
                    /
                    desiredSpeed
                );

        }



        const Vec3 horizontalVelocity(
            velocity.x,
            0.0f,
            velocity.z
        );



        const Vec3 velocityDelta =
            clampedDesired
            -
            horizontalVelocity;



        const float accelerationLimit =
            moveAcceleration
            *
            deltaSeconds;



        const float velocityDeltaLength =
            velocityDelta.length();



        if (
            velocityDeltaLength
            >
            accelerationLimit
        ) {

            const Vec3 limitedDelta =
                velocityDelta
                *
                (
                    accelerationLimit
                    /
                    std::max(
                        velocityDeltaLength,
                        kEpsilon
                    )
                );



            velocity.x +=
                limitedDelta.x;



            velocity.z +=
                limitedDelta.z;

        } else {

            velocity.x =
                clampedDesired.x;



            velocity.z =
                clampedDesired.z;

        }



        if (
            jumpRequested
            &&
            grounded
        ) {

            velocity.y =
                jumpSpeed;



            grounded =
                false;

        }



        velocity.y +=
            kGravity
            *
            deltaSeconds;



        position +=
            velocity
            *
            deltaSeconds;



        const float groundY =
            kGroundHeight
            +
            halfHeight
            +
            radius;



        if (
            position.y <= groundY
        ) {

            position.y =
                groundY;



            if (
                velocity.y < 0.0f
            ) {

                velocity.y =
                    0.0f;

            }



            grounded =
                true;

        } else {

            grounded =
                false;

        }

    }

};



struct VehicleDamageState {

    float health =
        1000.0f;



    float engineHealth =
        100.0f;



    std::array<float, 4> wheelHealth {

        100.0f,

        100.0f,

        100.0f,

        100.0f

    };



    float suspensionHealth =
        100.0f;



    void reset() {

        health =
            1000.0f;



        engineHealth =
            100.0f;



        wheelHealth.fill(
            100.0f
        );



        suspensionHealth =
            100.0f;

    }



    void applyImpact(
        float impactEnergy,
        const Vec3& localImpactPoint
    ) {

        const float safeEnergy =
            std::max(
                0.0f,
                impactEnergy
            );



        const float bodyDamage =
            safeEnergy
            *
            0.055f;



        health =
            std::max(
                0.0f,
                health
                -
                bodyDamage
            );



        engineHealth =
            std::max(
                0.0f,
                engineHealth
                -
                bodyDamage
                *
                (
                    0.35f
                    +
                    std::fabs(
                        localImpactPoint.x
                    )
                    *
                    0.12f
                )
            );



        const std::size_t wheelIndex =
            localImpactPoint.z >= 0.0f
                ? (
                    localImpactPoint.x >= 0.0f
                        ? 0
                        : 2
                )
                : (
                    localImpactPoint.x >= 0.0f
                        ? 1
                        : 3
                );



        wheelHealth[wheelIndex] =
            std::max(
                0.0f,
                wheelHealth[wheelIndex]
                -
                safeEnergy
                *
                0.08f
            );



        suspensionHealth =
            std::max(
                0.0f,
                suspensionHealth
                -
                safeEnergy
                *
                0.025f
            );

    }



    float driveMultiplier() const {

        return
            std::clamp(
                engineHealth / 100.0f,
                0.0f,
                1.0f
            );

    }



    bool isDestroyed() const {

        return
            health <= 0.0f
            ||
            engineHealth <= 0.0f;

    }

};



struct DestructibleObjectState {

    Aabb originalBounds;



    std::vector<FracturePiece> pieces;



    float health =
        100.0f;



    void reset(
        const Aabb& bounds
    ) {

        originalBounds =
            bounds;



        pieces.clear();



        pieces.push_back(
            FracturePiece {
                bounds,
                Vec3(
                    0.0f,
                    1.0f,
                    0.0f
                ),
                0.0f
            }
        );



        health =
            100.0f;

    }



    void applyImpact(
        const Vec3& impactPoint,
        const Vec3& impactNormal,
        float impactEnergy
    ) {

        if (
            pieces.empty()
            ||
            impactEnergy <= 0.0f
        ) {

            return;

        }



        health =
            std::max(
                0.0f,
                health
                -
                impactEnergy
                *
                0.08f
            );



        if (
            impactEnergy >= 18.0f
            &&
            pieces.size() < 16
        ) {

            std::vector<FracturePiece>
                fracturedPieces;



            for (
                const FracturePiece& piece
                :
                pieces
            ) {

                std::vector<FracturePiece>
                    localPieces =
                        fractureAabbByImpactPlane(
                            piece.bounds,
                            impactPoint,
                            impactNormal,
                            impactEnergy
                            /
                            static_cast<float>(
                                std::max(
                                    std::size_t(1),
                                    pieces.size()
                                )
                            )
                        );



                fracturedPieces.insert(
                    fracturedPieces.end(),
                    localPieces.begin(),
                    localPieces.end()
                );

            }



            pieces =
                std::move(
                    fracturedPieces
                );

        }

    }



    bool destroyed() const {

        return
            health <= 0.0f;

    }

};



class PhysicsWorld {

public:

    PhysicsWorld() {

        body.position =
            Vec3(
                0.0f,
                0.5f,
                0.0f
            );



        body.orientation =
            Quaternion();



        vehicle.initialize();



        character.reset(
            Vec3(
                0.0f,
                1.0f,
                2.0f
            )
        );



        vehicleDamage.reset();



        destructibleObject.reset(
            Aabb {
                Vec3(
                    -1.5f,
                    -0.5f,
                    -1.5f
                ),
                Vec3(
                    1.5f,
                    2.5f,
                    1.5f
                )
            }
        );

    }



    void reset() {

        body =
            RigidBody();



        body.position =
            Vec3(
                0.0f,
                0.5f,
                0.0f
            );



        body.orientation =
            Quaternion();



        vehicle =
            VehicleState();



        vehicle.initialize();



        character.reset(
            Vec3(
                0.0f,
                1.0f,
                2.0f
            )
        );



        vehicleDamage.reset();



        destructibleObject.reset(
            Aabb {
                Vec3(
                    -1.5f,
                    -0.5f,
                    -1.5f
                ),
                Vec3(
                    1.5f,
                    2.5f,
                    1.5f
                )
            }
        );



        simulationFrame =
            0;

    }



    std::array<float, 36> step(
        const Vec3& inputPosition,
        const Vec3& inputVelocity,
        float deltaSeconds
    ) {

        if (
            !initializedFromInput
        ) {

            body.position =
                inputPosition;



            body.linearVelocity =
                inputVelocity;



            initializedFromInput =
                true;

        }



        body.integrate(
            deltaSeconds
        );



        vehicle.linearVelocity =
            body.linearVelocity;



        vehicle.throttle =
            std::clamp(
                inputVelocity.x * 0.25f,
                -1.0f,
                1.0f
            )
            *
            vehicleDamage.driveMultiplier();



        vehicle.linearVelocity =
            body.linearVelocity;



        vehicle.simulate(
            deltaSeconds
        );



        character.step(
            Vec3(
                inputVelocity.x,
                0.0f,
                inputVelocity.z
            ),
            false,
            deltaSeconds
        );



        body.applyTorque(
            Vec3(
                0.0f,
                vehicle.yawRate * 0.015f,
                0.0f
            )
        );



        body.angularVelocity.x =
            vehicle.wheels[0].steeringAngle
            *
            0.05f;



        body.angularVelocity.y =
            vehicle.wheels[0].steeringAngle
            *
            0.08f;



        body.angularVelocity.z =
            0.0f;



        const Mat4 transform =
            makeTransformMatrix(
                body.position,
                body.orientation
            );



        std::array<float, 36> output {};



        output[0] =
            body.position.x;



        output[1] =
            body.position.y;



        output[2] =
            body.position.z;



        output[3] =
            body.linearVelocity.x;



        output[4] =
            body.linearVelocity.y;



        output[5] =
            body.linearVelocity.z;



        output[6] =
            body.orientation.x;



        output[7] =
            body.orientation.y;



        output[8] =
            body.orientation.z;



        output[9] =
            body.orientation.w;



        output[10] =
            body.angularVelocity.x;



        output[11] =
            body.angularVelocity.y;



        output[12] =
            body.angularVelocity.z;



        for (
            std::size_t index = 0;
            index < 16;
            ++index
        ) {

            output[
                13 + index
            ] =
                transform.values[index];

        }



        output[29] =
            vehicle.linearVelocity.length();



        output[30] =
            vehicle.engineRpm;



        output[31] =
            vehicleDamage.health;



        output[32] =
            character.grounded
                ? 1.0f
                : 0.0f;



        output[33] =
            character.velocity.length();



        output[34] =
            destructibleObject.health;



        output[35] =
            static_cast<float>(
                destructibleObject.pieces.size()
            );



        ++simulationFrame;



        return output;

    }



    std::uint64_t getFrame() const {

        return simulationFrame;

    }



private:

    RigidBody body;



    VehicleState vehicle;



    VehicleDamageState vehicleDamage;



    CharacterController character;



    DestructibleObjectState destructibleObject;



    bool initializedFromInput =
        false;



    std::uint64_t simulationFrame =
        0;

};



std::mutex gPhysicsMutex;



PhysicsWorld gPhysicsWorld;



std::string makeEngineVersion() {

    return
        "Aether3D Native Physics 3.0.0 / C++20 / Android NDK / RigidBody + Vehicle + Character + IK + Destruction";

}



}  // namespace



extern "C"
JNIEXPORT jstring JNICALL
Java_com_aether3d_studio_MainActivity_nativeGetEngineVersion(
    JNIEnv* env,
    jclass
) {

    const std::string version =
        makeEngineVersion();



    return env->NewStringUTF(
        version.c_str()
    );

}



extern "C"
JNIEXPORT jfloatArray JNICALL
Java_com_aether3d_studio_MainActivity_nativeStepSimulation(
    JNIEnv* env,
    jclass,
    jfloatArray positions,
    jfloatArray velocities,
    jfloat deltaSeconds
) {

    if (
        positions == nullptr
        ||
        velocities == nullptr
    ) {

        return nullptr;

    }



    const jsize positionLength =
        env->GetArrayLength(
            positions
        );



    const jsize velocityLength =
        env->GetArrayLength(
            velocities
        );



    if (
        positionLength < 3
        ||
        velocityLength < 3
    ) {

        return nullptr;

    }



    jfloat positionValues[3] {};

    jfloat velocityValues[3] {};



    env->GetFloatArrayRegion(
        positions,
        0,
        3,
        positionValues
    );



    env->GetFloatArrayRegion(
        velocities,
        0,
        3,
        velocityValues
    );



    const float safeDelta =
        std::clamp(
            deltaSeconds,
            0.0001f,
            0.0333333f
        );



    const Vec3 inputPosition(
        positionValues[0],
        positionValues[1],
        positionValues[2]
    );



    const Vec3 inputVelocity(
        velocityValues[0],
        velocityValues[1],
        velocityValues[2]
    );



    std::array<float, 36> output {};



    {

        std::lock_guard<std::mutex> lock(
            gPhysicsMutex
        );



        output =
            gPhysicsWorld.step(
                inputPosition,
                inputVelocity,
                safeDelta
            );

    }



    jfloatArray result =
        env->NewFloatArray(
            static_cast<jsize>(
                output.size()
            )
        );



    if (
        result == nullptr
    ) {

        return nullptr;

    }



    env->SetFloatArrayRegion(
        result,
        0,
        static_cast<jsize>(
            output.size()
        ),
        output.data()
    );



    __android_log_print(
        ANDROID_LOG_DEBUG,
        kLogTag,
        "physics frame=%llu position=(%.3f,%.3f,%.3f)",
        static_cast<unsigned long long>(
            gPhysicsWorld.getFrame()
        ),
        output[0],
        output[1],
        output[2]
    );



    return result;

}



extern "C"
JNIEXPORT void JNICALL
Java_com_aether3d_studio_MainActivity_nativeResetSimulation(
    JNIEnv*,
    jclass
) {

    std::lock_guard<std::mutex> lock(
        gPhysicsMutex
    );



    gPhysicsWorld.reset();



    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "physics world reset"
    );

}
