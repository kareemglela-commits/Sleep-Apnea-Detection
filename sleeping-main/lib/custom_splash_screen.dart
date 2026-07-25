import 'dart:async';
import 'package:flutter/material.dart';
import 'package:sleeping/widgets/home_view.dart'; // Adjust the import according to your project structure

class SplashScreen extends StatefulWidget {
  const SplashScreen({super.key});

  @override
  _SplashScreenState createState() => _SplashScreenState();
}

class _SplashScreenState extends State<SplashScreen> {
  @override
  void initState() {
    super.initState();
    Timer(const Duration(seconds: 2), () {
      Navigator.of(context).pushReplacement(
        MaterialPageRoute(builder: (context) => const HomeView()),
      );
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.white,
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Row(
              children: [
                Expanded(
                  child: Image.asset(
                    'assets/images/bio.jpeg',
                  ),
                ), // Replace with your image paths
                const SizedBox(width: 20),
                Expanded(
                  child: Image.asset(
                    'assets/images/eng.jpeg',
                  ),
                ),
              ],
            ),
            const SizedBox(height: 20),
            const Text(
              'BIOMEDICAL ENGINEERING',
              style: TextStyle(
                fontSize: 24,
                fontWeight: FontWeight.bold,
              ),
            ),
            const SizedBox(height: 20),
            const Text(
              'Apnea detection',
              style: TextStyle(
                fontSize: 20,
                fontWeight: FontWeight.w500,
              ),
            ),
            // const SizedBox(height: 20),
            Image.asset(
              'assets/images/heartbeat.jpg',
            ),
          ],
        ),
      ),
    );
  }
}
