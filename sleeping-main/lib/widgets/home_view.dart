import 'package:audioplayers/audioplayers.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:sleeping/widgets/custom_divider.dart';
import 'package:sleeping/widgets/custom_info_dialog.dart';
import 'package:sleeping/widgets/graph_view.dart';
import 'package:sleeping/widgets/linear_char.dart';
import 'package:sleeping/widgets/temp_linear.dart';

class HomeView extends StatefulWidget {
  const HomeView({super.key});

  @override
  State<HomeView> createState() => _HomeViewState();
}

class _HomeViewState extends State<HomeView> {

  final databaseReference = FirebaseDatabase.instance.ref('Data');
  String audioLink = 'sound.wav';
  final player = AudioPlayer();

  playSound() async {
    player.play(AssetSource(audioLink));
  }

  List<SalesData> gsrGraphData = [
    SalesData('0', 0),
  ];

  List<SalesData> ecgGraphData = [
    SalesData('0', 0),
  ];

  String time = '0';
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Apnea Detection'),
        centerTitle: true,
        automaticallyImplyLeading: false,
        actions: [
          IconButton(
            onPressed: () {
              showDialog(
                context: context,
                builder: (context) => AlertDialog(
                  title: const Text('Sleep apnea'),
                  content: const SingleChildScrollView(
                    child: CustomInfoDialog(),
                  ),
                  actions: [
                    TextButton(
                      onPressed: () {
                        Navigator.pop(context);
                      },
                      child: const Text('Close'),
                    ),
                  ],
                ),
              );
            },
            icon: const Icon(Icons.info),
          ),
        ],
      ),
      body: Center(
        child: Padding(
          padding: const EdgeInsets.all(24.0),
          child: SingleChildScrollView(
            child: StreamBuilder(
              stream: databaseReference.onValue,
              builder: (context, snapshot) {
                time = (int.parse(time) + 1).toString();
                if (snapshot.hasData) {
                  dynamic data = snapshot.data!.snapshot.value;
                  gsrGraphData = [
                    ...gsrGraphData,
                    SalesData(
                      time,
                      double.tryParse(data['GSR']) ?? 0,
                    ),
                  ];
                  ecgGraphData = [
                    ...ecgGraphData,
                    SalesData(
                      time,
                      double.tryParse(data['ECG']) ?? 0,
                    ),
                  ];
                  if (data['Apnea']) {
                    playSound();
                    WidgetsBinding.instance.addPostFrameCallback(
                      (_) {
                        showDialog(
                          context: context,
                          builder: (context) {
                            return AlertDialog(
                              title: const Text('Sleep Apnea Detected'),
                              content: const Text('Please consult a doctor'),
                              actions: [
                                TextButton(
                                  onPressed: () async {
                                    await player.stop();
                                    Navigator.pop(context);
                                  },
                                  child: const Text('Close'),
                                ),
                              ],
                            );
                          },
                        );
                      },
                    );
                  }
                  return Column(
                    children: [
                      GraphView(
                        graphData: ecgGraphData,
                      ),
                      Text(
                          'ECG: ${data['ECG']}',
                      ),
                      const CustomDivider(),
                      RadialChar(
                        maxRange: 120,
                        value: double.tryParse(data['SpO2']) ?? 0,
                        title: 'SpO2',
                      ),
                      const CustomDivider(),
                      const Icon(
                        Icons.monitor_heart,
                        color: Colors.red,
                        size: 120,
                      ),
                      Text(
                        data['BPM'] + ' BPM',
                        style: const TextStyle(
                          fontSize: 20,
                        ),
                      ),
                      const CustomDivider(),
                      TempLinear(
                        value: double.tryParse(data['Temp']) ?? 0,
                      ),
                      const CustomDivider(),
                      GraphView(
                        graphData: gsrGraphData,
                      ),
                      Text(
                        'GSR: ${data['GSR']}',
                      ),
                    ],
                  );
                } else if (snapshot.hasError) {
                  return Center(
                    child: Text('Error : ${snapshot.error}'),
                  );
                } else if (snapshot.connectionState ==
                    ConnectionState.waiting) {
                  return const Center(
                    child: CircularProgressIndicator(),
                  );
                }
                return const SizedBox();
              },
            ),
          ),
        ),
      ),
    );
  }
}
