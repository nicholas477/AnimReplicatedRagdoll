# What is this?
This plugin adds in a component and an animation node that replicates entire skeletal mesh ragdolls.

## How does it work?
On the server side the skeletal mesh is simulated as usual and the bone transforms are captured on tick. The client side interps each bone to the replicated bone transform inside the animation node.

### Networking details
The plugin has some network optimizations. If a bone hasn't changed its transform more than 1cm of translation or 1 degree of rotation then it is not updated over the network.

Network transform updates are compressed. Translations are compressed using `FVector_NetQuantize10` and rotations are packed to 1 byte per axis. Bone scales are NOT replicated.

_(TODO: @nicholas477 add support for variable precision network compression)_

# How to use
1. Add the `Replicated Ragdoll` anim node to your skeletal mesh's animation BP and make sure your skeletal mesh is using that animation blueprint.
![Alt text](Demonstration.PNG?raw=true "Title")  
  - The node should be at the end of your animation blueprint.
2. Add the `ReplicatedRagdollComponent` to your actor. Make sure it is directly attached to your skeletal mesh component.
3. Make sure that your actor that contains the skeletal mesh is replicated.
4. Make your skeletal mesh simulate physics on server and it will capture and replicate the ragdoll to the client. The client *should not* simulate physics. The replicated ragdoll data should be applied on client as long as `ShouldApplyRagdoll` returns true on the component and there is data to apply.
