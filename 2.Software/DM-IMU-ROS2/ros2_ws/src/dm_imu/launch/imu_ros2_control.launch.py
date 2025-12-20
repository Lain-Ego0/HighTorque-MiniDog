from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, Command
from launch_ros.actions import Node
import os

def generate_launch_description():
    port_arg = DeclareLaunchArgument('port', default_value='/dev/ttyACM0')
    baud_arg = DeclareLaunchArgument('baud', default_value='921600')
    frame_arg = DeclareLaunchArgument('frame_id', default_value='imu_link')
    simulate_arg = DeclareLaunchArgument('simulate', default_value='false')

    robot_description = Command([
        'xacro ', os.path.join(os.path.dirname(__file__), '..', 'urdf', 'dmbot_imu.urdf.xacro'),
        ' port:=', LaunchConfiguration('port'),
        ' baud:=', LaunchConfiguration('baud'),
        ' frame_id:=', LaunchConfiguration('frame_id'),
        ' simulate:=', LaunchConfiguration('simulate')
    ])

    state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description': robot_description}],
    )

    control_node = Node(
        package='controller_manager',
        executable='ros2_control_node',
        parameters=[{'robot_description': robot_description},
                    os.path.join(os.path.dirname(__file__), '..', 'config', 'imu_broadcaster.yaml')]
    )

    spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=[
            'imu_broadcaster',
            '--controller-type', 'imu_sensor_broadcaster/IMUSensorBroadcaster',
        ],
    )

    return LaunchDescription([
        port_arg,
        baud_arg,
        frame_arg,
        simulate_arg,
        state_publisher,
        control_node,
        spawner,
    ])
