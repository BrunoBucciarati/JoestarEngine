struct SH9 {
    float coefs[9];
};

SH9 ProjectOntoSH9(vec3 dir, uint level)
{
    SH9 sh;
    // Band 0
    sh.coefs[0] = 0.282095f;

    // Band 1
    sh.coefs[1] = 0.488603f * dir.y;
    sh.coefs[2] = 0.488603f * dir.z;
    sh.coefs[3] = 0.488603f * dir.x;

    // Band 2
    if (level > 1) {
        sh.coefs[4] = 1.092548f * dir.x * dir.y;
        sh.coefs[5] = 1.092548f * dir.y * dir.z;
        sh.coefs[6] = 0.315392f * (3.0f * dir.z * dir.z - 1.0f);
        sh.coefs[7] = 1.092548f * dir.x * dir.z;
        sh.coefs[8] = 0.546274f * (dir.x * dir.x - dir.y * dir.y);
    }

    return sh;
}
//