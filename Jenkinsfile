#!groovy
/**
 * Nightly tests
 * Clean build (not incremental)
 * Run everything
 */
node {
	try{
	    stage('Git Fetch') { // for display purposes
	       build job: 'Git_Fetch';
	    }
	    
	    stage('Build CMake') {
	       build job: 'Build_CMake_GCC';
        }
        
	    stage('Build Make') {
		   build job: 'Build_GCC';
	    }

	    stage('CppCheck Test') {
	       build job: 'CppCheck_Test';
	    }  

	    stage('Integration Test') {
	       build job: 'Integration_Test';
	    } 

	    stage('Test Integration Memchek (Valgrind)') {
	       build job: 'Test_Integration_Memchek_Valgrind';
	    }

        notifySuccessful();

    } catch (e) {
        currentBuild.result = "FAILED";
        notifyFailed();
        throw e;
    }
}

def notifySuccessful() { 
    // Do nothing
}

def notifyFailed() {
  emailext (
      subject: "[WSNET][Jenkins] FAILED: Job '${env.JOB_NAME} [${env.BUILD_NUMBER}]'",
      body: """<p>FAILED: Job '${env.JOB_NAME} [${env.BUILD_NUMBER}]':</p>
        <p>Check console output at &QUOT;<a href='${env.BUILD_URL}'>${env.JOB_NAME} [${env.BUILD_NUMBER}]</a>&QUOT;</p>""",
      to: "Luiz.SURATYFILHO@cea.fr"
    )
}