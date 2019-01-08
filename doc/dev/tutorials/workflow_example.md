# Workflow tutorial

As indicated in the contributing guide, you should always start by creating an issue in the project. 
After the discussions with your colleagues, if you are assigned to the issue, you can create a new branch and start to implement your own solution.

Now that you created your branch on the github server, you should first "download" 
it to your local Git repository. Note: from now on, we will refer to the github server as remote. In order to "download" the remote, you should first fetch its elements:

```shell
$ git fetch --all
```
It will produce a result similar to this:

```shell
Fetching origin
remote: Counting objects: 32, done.
remote: Compressing objects: 100% (32/32), done.
remote: Total 32 (delta 12), reused 2 (delta 0)
Unpacking objects: 100% (32/32), done.
From git-dsys.intra.cea.fr:WSNET/wsnet
 * [new branch]      31-create-workflow-example -> upstream/31-create-workflow-example
```

This means you have downloaded the status of the branch ```31-create-workflow-example``` from remote ```origin```, i.e. the branch that is located in the project ```wsnet``` at the github server. 
However, you still don't have a local branch on which you can work on and perform your commits. Thus, in order to create it:

```shell
$ git checkout -b 31-create-workflow-example origin/31-create-workflow-example
```

This command will create a new branch based on the one in the remote and switch to this new branch, as the output clearly indicates:

```shell
Branch 31-create-workflow-example set up to track remote branch 31-create-workflow-example from origin.
Switched to a new branch '31-create-workflow-example'
```
With the ```git checkout -b``` command you created a new local branch, called ```31-create-workflow-example```, 
which is based on the one at the github server (```origin/31-create-workflow-example```).  
Now, you can start your own modifications locally. Any commit message you create will be stored at your local branch. After your several commits, whenever you feel your code is ready, you can push it to the remote. 

## Push directly to forked ```USER_NAME/wsnet``` project

Note that you could push the information to your forked project instead of the ```CEA-Leti/wsnet``` project. In this way, at the end of the process you should later create a pull request from the branch located in your forked project (```USER_NAME/wsnet```) to the ```WSNET/wsnet``` project.

## Update branch information locally

After you merged, it is likely the branch is deleted in remote, you need to update this information on your local repository.

```shell
$ git fetch --all
$ git checkout master
$ git merge upstream/master
$ git branch -d 31-create-workflow-example
$ git fetch -p
```
